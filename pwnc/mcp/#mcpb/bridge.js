/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

"use strict";

//==================================================================================================
//   MCP STDIO BRIDGE
//==================================================================================================

const http     = require("node:http");
const https    = require("node:https");
const readline = require("node:readline");

//config
const TARGET   = process.env.PWNC_URL   || "";
const TOKEN    = process.env.PWNC_TOKEN || "";
const VERSION  = "2026-07-28";

//in-flight requests by json-rpc id
const pending = new Map();

function send(value)
{
    process.stdout.write(JSON.stringify(value) + "\n");
};

function log(message)
{
    process.stderr.write("PWNC: " + message + "\n");
};

function error(id, code, message)
{
    //notifications take no reply
    if ((id === undefined) || (id === null)) return log(message);

    send({jsonrpc: "2.0", id: id, error: {code: code, message: message}});
};

function header_value(value)
{
    return (/^[\x21-\x7E]+$/.test(value) && ! /^=\?base64\?.*\?=$/.test(value)) ?
           value : "=?base64?" + Buffer.from(value, "utf8").toString("base64") + "?=";
};

function mirror_name(json)
{
    const param = json.params || {};

    //methods mirroring name or uri
    switch (json.method)
    {
    case "tools/call":
    case "prompts/get":

        return param.name;

    case "resources/read":

        return param.uri;
    };

    return undefined;
};

function forward(json)
{
    const id     = json.id;
    const body   = Buffer.from(JSON.stringify(json), "utf8");
    const target = new URL(TARGET);
    const meta   = (json.params || {})._meta || {};

    //mirror body fields into request metadata headers
    const header = {
        "Content-Type":         "application/json",
        "Content-Length":       body.length,
        "Accept":               "application/json, text/event-stream",
        "Authorization":        "Bearer " + TOKEN,
        "MCP-Protocol-Version": meta["io.modelcontextprotocol/protocolVersion"] || VERSION,
        "MCP-Method":           json.method};

    const name = mirror_name(json);
    if (name !== undefined) header["MCP-Name"] = header_value(String(name));

    //send request
    const request = ((target.protocol === "http:") ? http : https).request(
        target,
        {method: "POST", headers: header},
        (response) =>
        {
            const type = (response.headers["content-type"] || "").split(";")[0].trim().toLowerCase();
            let   data = "";

            response.setEncoding("utf8");
            response.on("data", (value) => { data += value; });
            response.on("end", () =>
            {
                pending.delete(id);

                //notification accepted, no body
                if (response.statusCode === 202) return;

                //response streams not implemented
                if (type === "text/event-stream")
                    return error(id, -32603, "Endpoint returned response stream.");

                //forward reply
                try
                {
                    send(JSON.parse(data));
                }
                catch (exception)
                {
                    error(id, -32700, "Invalid reply from " + target.host + ".");
                };
            });
        });

    request.on("error", (exception) =>
    {
        pending.delete(id);

        //cancelled by client
        if (request.cancelled) return;

        error(id, -32603, exception.message);
    });

    //only requests can be cancelled
    if ((id !== undefined) && (id !== null)) pending.set(id, request);

    request.end(body);
};

//==================================================================================================
//   MAIN
//==================================================================================================

if (TARGET === "") { log("PWNC_URL is not set.");   process.exit(1); };
if (TOKEN  === "") { log("PWNC_TOKEN is not set."); process.exit(1); };

readline.createInterface({input: process.stdin})

.on("line", (line) =>
{
    if (line.trim() === "") return;

    let json;

    try
    {
        json = JSON.parse(line);
    }
    catch (exception)
    {
        return log("Parse error on input.");
    };

    //requests and notifications only
    if (typeof json.method !== "string") return log("Discarded input without method.");

    //cancellation closes request
    if (json.method === "notifications/cancelled")
    {
        const request = pending.get((json.params || {}).requestId);

        if (request)
        {
            pending.delete((json.params || {}).requestId);
            request.cancelled = true;
            request.destroy();
        };

        return;
    };

    forward(json);
})

//end of input terminates bridge
.on("close", () => process.exit(0));