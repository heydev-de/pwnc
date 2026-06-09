/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

function sha256(str)
{
    var rot    = function(v, n) { return (v >>> n) | (v << (32 - n)); };
    var hash   = [0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19];
    var k      = [0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2];
    var w      = [];
    var c      = str.length * 8;
    var result = "";
    var i, j;

    str += "\x80";
    str += "\0".repeat((56 - (str.length % 64)) % 64);

    for (i = 0; i < str.length; i++)
    {
        j = str.charCodeAt(i);
        if (j >> 8) return;
        w[i >> 2] |= j << ((3 - i) % 4) * 8;
    };

    w[w.length] = (c / 4294967296) | 0;
    w[w.length] = c;

    for (i = 0; i < w.length; )
    {
        var _w    = w.slice(i, i += 16);
        var _hash = hash.slice();

        for (j = 0; j < 64; j++)
        {
            var w2  = _w[j - 2];
            var w15 = _w[j - 15];
            var h0  = hash[0];
            var h1  = hash[1];
            var h2  = hash[2];
            var h4  = hash[4];

            if (j >= 16)
                _w[j] = (_w[j - 16] +
                    (rot(w15, 7) ^ rot(w15, 18) ^ (w15 >>> 3)) +
                    _w[j - 7] +
                    (rot(w2, 17) ^ rot(w2, 19) ^ (w2 >>> 10))) | 0;

            var tmp1 = hash[7] +
                       (rot(h4, 6) ^ rot(h4, 11) ^ rot(h4, 25)) +
                       ((h4 & hash[5]) ^ ((~h4) & hash[6])) +
                       k[j] + _w[j];

            var tmp2 = (rot(h0, 2) ^ rot(h0, 13) ^ rot(h0, 22)) +
                       ((h0 & h1) ^ (h0 & h2) ^ (h1 & h2));

            hash[7] = hash[6];
            hash[6] = hash[5];
            hash[5] = hash[4];
            hash[4] = hash[3];
            hash[3] = hash[2];
            hash[2] = hash[1];
            hash[1] = hash[0];
            hash[0] = (tmp1 + tmp2) | 0;
            hash[4] = (hash[4] + tmp1) | 0;
        };

        for (j = 0; j < 8; j++) hash[j] = (hash[j] + _hash[j]) | 0;
    };

    var b;
    for (i = 0; i < 8; i++)
    {
        for (j = 3; j + 1; j--)
        {
            b       = (hash[i] >> (j * 8)) & 255;
            result += ((b < 16) ? 0 : "") + b.toString(16);
        };
    };

    return result;
};