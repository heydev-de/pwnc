<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   PROFILE DATA CLASS
//==================================================================================================

class profile_data
{

//properties
//--------------------------------------------------------------------------------------------------

    public $id                    = NULL;
    public $time_created          = NULL;
    public $time_updated          = NULL;

    //administrative data
    public $code                  = NULL;
    public $user                  = NULL;
    public $password              = NULL;
    public $superuser             = NULL;
    public $enabled               = NULL;

    //personal data
    public $company               = NULL;
    public $prename               = NULL;
    public $surname               = NULL;
    public $street                = NULL;
    public $zipcode               = NULL;
    public $city                  = NULL;
    public $country               = NULL;

    //contact details
    public $phone1                = NULL;
    public $phone2                = NULL;
    public $mobile                = NULL;
    public $fax                   = NULL;
    public $email                 = NULL;
    public $url                   = NULL;

    //bank details
    public $account_number        = NULL;
    public $financial_institution = NULL;
    public $bankcode              = NULL;
    public $account_holder        = NULL;

    //credit card information
    public $credit_card_number    = NULL;
    public $credit_institute      = NULL;
    public $credit_card_holder    = NULL;
    public $credit_card_validity  = NULL;

    //miscellaneous data
    public $comment               = NULL;
    public $field1                = NULL;
    public $field2                = NULL;
    public $field3                = NULL;
    public $field4                = NULL;
    public $field5                = NULL;
    public $field6                = NULL;
    public $field7                = NULL;
    public $field8                = NULL;
    public $field9                = NULL;
    public $field10               = NULL;
    public $field11               = NULL;
    public $field12               = NULL;
    public $field13               = NULL;
    public $field14               = NULL;
    public $field15               = NULL;
    public $field16               = NULL;
    public $field17               = NULL;
    public $field18               = NULL;
    public $field19               = NULL;
    public $field20               = NULL;

    //generic
    public $name                  = NULL;
    public $address               = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        //index
        $this->id = (int)$this->id;

        //administrative data
        $this->code = stripspaces($this->code);
        if (stre($this->code)) $this->code = unique_id();

        //personal data
        $this->company = stripspaces($this->company);
        $this->prename = stripspaces($this->prename);
        $this->surname = stripspaces($this->surname);
        $this->street  = stripspaces($this->street);
        $this->zipcode = stripspaces($this->zipcode);
        $this->city    = stripspaces($this->city);
        $this->country = stripspaces($this->country);

        //contact details
        $this->phone1 = stripspaces($this->phone1);
        $this->phone2 = stripspaces($this->phone2);
        $this->mobile = stripspaces($this->mobile);
        $this->fax    = stripspaces($this->fax);
        $this->email  = stripspaces($this->email);
        $this->url    = utf8_trim($this->url);

        //administrative data
        if (stre($this->user)) $this->user = stre($this->email) ? $this->code : $this->email;

        //bank details
        $this->account_number        = stripspaces($this->account_number);
        $this->financial_institution = stripspaces($this->financial_institution);
        $this->bankcode              = stripspaces($this->bankcode);
        $this->account_holder        = stripspaces($this->account_holder);
        if (stre($this->account_holder))
            $this->account_holder = trim($this->prename . " " . $this->surname);

        //credit card information
        $this->credit_card_number = stripspaces($this->credit_card_number);
        $this->credit_institute   = stripspaces($this->credit_institute);
        $this->credit_card_holder = stripspaces($this->credit_card_holder);
        if (stre($this->credit_card_holder))
            $this->credit_card_holder = trim($this->prename . " " . $this->surname);

        $time = time();
        if (preg_match("/^([^\/]{1,2})\/(.{1,4})$/", $this->credit_card_validity, $value))
        {
            //month
            $value[1] = str_pad($value[1], 2, "0", STR_PAD_LEFT);
            if (! preg_match("/^(?:01|02|03|04|05|06|07|08|09|10|11|12)$/", $value[1]))
                $value[1] = date("m", $time);

            //year
            $year     = date("Y", $time);
            $value[2] = str_pad($value[2], 4, $year, STR_PAD_LEFT);
            if (! preg_match("/^[\d]{4}$/", $value[2]))
                $value[2] = $year;

            $this->credit_card_validity = $value[1] . "/" . $value[2];
        }
        else
        {
            $this->credit_card_validity = date("m", $time) . "/" . date("Y", $time);
        };

        //miscellaneous data
        $this->comment = utf8_trim($this->comment);

        for ($i = 1; $i < 21; $i++)
            $this->{"field$i"} = stripspaces($this->{"field$i"});

        //generic
        $this->name    = (stre($this->prename) || stre($this->surname)) ?
                         $this->prename . $this->surname :
                         $this->surname . ", " . $this->prename;
        $this->address = "";
        if (nstre($this->street))
            $this->address = $this->street . "\n";
        if (nstre($this->zipcode))
        {
            $this->address .= $this->zipcode;
            if (nstre($this->city))
            {
                $this->address .= " " . $this->city;
                if (nstre($this->country))
                    $this->address .= "\n" . $this->country;
            }
            elseif (nstre($this->country))
            {
                $this->address .= " " . $this->country;
            };
        }
        elseif (nstre($this->city))
        {
            $this->address .= $this->city;
            if (nstre($this->country))
                $this->address .= "\n" . $this->country;
        }
        elseif (nstre($this->country))
        {
            $this->address .= $this->country;
        };
    }
};

//==================================================================================================
//   PROFILE CLASS
//==================================================================================================

//permission
define("CMS_PROFILE_PERMISSION_OPERATOR",      "operator");

//database
define("CMS_DB_PROFILE",                       CMS_DB_PREFIX . "profile");
define("CMS_DB_PROFILE_INDEX",                 "id");
define("CMS_DB_PROFILE_TIME_CREATED",          "time_created");
define("CMS_DB_PROFILE_TIME_UPDATED",          "time_updated");
define("CMS_DB_PROFILE_CODE",                  "code");
define("CMS_DB_PROFILE_USER",                  "user");
define("CMS_DB_PROFILE_PASSWORD",              "password");
define("CMS_DB_PROFILE_SUPERUSER",             "superuser");
define("CMS_DB_PROFILE_ENABLED",               "enabled");
define("CMS_DB_PROFILE_COMPANY",               "company");
define("CMS_DB_PROFILE_PRENAME",               "prename");
define("CMS_DB_PROFILE_SURNAME",               "surname");
define("CMS_DB_PROFILE_STREET",                "street");
define("CMS_DB_PROFILE_ZIPCODE",               "zipcode");
define("CMS_DB_PROFILE_CITY",                  "city");
define("CMS_DB_PROFILE_COUNTRY",               "country");
define("CMS_DB_PROFILE_PHONE_1",               "phone1");
define("CMS_DB_PROFILE_PHONE_2",               "phone2");
define("CMS_DB_PROFILE_MOBILE",                "mobile");
define("CMS_DB_PROFILE_FAX",                   "fax");
define("CMS_DB_PROFILE_EMAIL",                 "email");
define("CMS_DB_PROFILE_URL",                   "url");
define("CMS_DB_PROFILE_ACCOUNT_NUMBER",        "account_number");
define("CMS_DB_PROFILE_FINANCIAL_INSTITUTION", "financial_institution");
define("CMS_DB_PROFILE_BANKCODE",              "bankcode");
define("CMS_DB_PROFILE_ACCOUNT_HOLDER",        "account_holder");
define("CMS_DB_PROFILE_CREDIT_CARD_NUMBER",    "credit_card_number");
define("CMS_DB_PROFILE_CREDIT_INSTITUTE",      "credit_institute");
define("CMS_DB_PROFILE_CREDIT_CARD_HOLDER",    "credit_card_holder");
define("CMS_DB_PROFILE_CREDIT_CARD_VALIDITY",  "credit_card_validity");
define("CMS_DB_PROFILE_COMMENT",               "comment");

define("CMS_DB_PROFILE_CUSTOM",                CMS_DB_PREFIX . "profile_custom");
define("CMS_DB_PROFILE_CUSTOM_INDEX",          "id");
define("CMS_DB_PROFILE_CUSTOM_FIELD",          "field");

class profile
{

//properties
//--------------------------------------------------------------------------------------------------

    public $operator = FALSE;
    public $enabled  = FALSE;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($override_permission = FALSE)
    {
        //establish database connection
        $mysql = new mysql();

        if (
             $mysql->verify_table(CMS_DB_PROFILE,
                                  [CMS_DB_PROFILE_INDEX                 => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_PROFILE_TIME_CREATED          => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_PROFILE_TIME_UPDATED          => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_PROFILE_CODE                  => "VARCHAR(40) NOT NULL DEFAULT '' UNIQUE",
                                   CMS_DB_PROFILE_USER                  => "VARCHAR(40) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '' UNIQUE",
                                   CMS_DB_PROFILE_PASSWORD              => "VARCHAR(64) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_SUPERUSER             => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_ENABLED               => "ENUM('1', '0') NOT NULL DEFAULT '1'",
                                   CMS_DB_PROFILE_COMPANY               => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_PRENAME               => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_SURNAME               => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_STREET                => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_ZIPCODE               => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CITY                  => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_COUNTRY               => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_PHONE_1               => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_PHONE_2               => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_MOBILE                => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_FAX                   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_EMAIL                 => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_URL                   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_ACCOUNT_NUMBER        => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_FINANCIAL_INSTITUTION => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_BANKCODE              => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_ACCOUNT_HOLDER        => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CREDIT_CARD_NUMBER    => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CREDIT_INSTITUTE      => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CREDIT_CARD_HOLDER    => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CREDIT_CARD_VALIDITY  => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_COMMENT               => "TEXT NOT NULL"])
             &&
             $mysql->verify_table(CMS_DB_PROFILE_CUSTOM,
                                  [CMS_DB_PROFILE_CUSTOM_INDEX          => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_PROFILE_CUSTOM_FIELD .  "1"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD .  "2"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD .  "3"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD .  "4"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD .  "5"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD .  "6"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD .  "7"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD .  "8"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD .  "9"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "10"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "11"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "12"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "13"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "14"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "15"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "16"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "17"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "18"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "19"   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_PROFILE_CUSTOM_FIELD . "20"   => "VARCHAR(255) NOT NULL DEFAULT ''"])
           )
        {
            $this->operator = $override_permission ? TRUE : cms_permission(CMS_PROFILE_PERMISSION_OPERATOR);
            $this->enabled  = TRUE;
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add(&$profile_data)
    {
        if (
             (! $this->enabled)
             ||
             (! $this->operator)
           )
            return FALSE;

        $profile_data->__construct();
        $time = sqlesc(time());

        if (mysql_query("INSERT " . CMS_DB_PROFILE . " " .
                        "(" .       CMS_DB_PROFILE_TIME_CREATED . ", " .
                                    CMS_DB_PROFILE_TIME_UPDATED . ", " .
                                    CMS_DB_PROFILE_CODE . ", " .
                                    CMS_DB_PROFILE_USER . ", " .
                                    CMS_DB_PROFILE_PASSWORD . ", " .
                                    CMS_DB_PROFILE_SUPERUSER . ", " .
                                    CMS_DB_PROFILE_ENABLED . ", " .
                                    CMS_DB_PROFILE_COMPANY . ", " .
                                    CMS_DB_PROFILE_PRENAME . ", " .
                                    CMS_DB_PROFILE_SURNAME . ", " .
                                    CMS_DB_PROFILE_STREET . ", " .
                                    CMS_DB_PROFILE_ZIPCODE . ", " .
                                    CMS_DB_PROFILE_CITY . ", " .
                                    CMS_DB_PROFILE_COUNTRY . ", " .
                                    CMS_DB_PROFILE_PHONE_1 . ", " .
                                    CMS_DB_PROFILE_PHONE_2 . ", " .
                                    CMS_DB_PROFILE_MOBILE . ", " .
                                    CMS_DB_PROFILE_FAX . ", " .
                                    CMS_DB_PROFILE_EMAIL . ", " .
                                    CMS_DB_PROFILE_URL . ", " .
                                    CMS_DB_PROFILE_ACCOUNT_NUMBER . ", " .
                                    CMS_DB_PROFILE_FINANCIAL_INSTITUTION . ", " .
                                    CMS_DB_PROFILE_BANKCODE . ", " .
                                    CMS_DB_PROFILE_ACCOUNT_HOLDER . ", " .
                                    CMS_DB_PROFILE_CREDIT_CARD_NUMBER . ", " .
                                    CMS_DB_PROFILE_CREDIT_INSTITUTE . ", " .
                                    CMS_DB_PROFILE_CREDIT_CARD_HOLDER . ", " .
                                    CMS_DB_PROFILE_CREDIT_CARD_VALIDITY . ", " .
                                    CMS_DB_PROFILE_COMMENT . ") " .
                        "VALUES (" .
                        "'" .       $time . "', " .
                        "'" .       $time . "', " .
                        "'" .       sqlesc($profile_data->code) . "', " .
                        "'" .       sqlesc($profile_data->user) . "', " .
                        "'" .       sqlesc(hash64($profile_data->password)) . "', " .
                        "'" .       sqlesc($profile_data->superuser) . "', " .
                        "'" .       sqlesc((bool)$profile_data->enabled) . "', " .
                        "'" .       sqlesc($profile_data->company) . "', " .
                        "'" .       sqlesc($profile_data->prename) . "', " .
                        "'" .       sqlesc($profile_data->surname) . "', " .
                        "'" .       sqlesc($profile_data->street) . "', " .
                        "'" .       sqlesc($profile_data->zipcode) . "', " .
                        "'" .       sqlesc($profile_data->city) . "', " .
                        "'" .       sqlesc($profile_data->country) . "', " .
                        "'" .       sqlesc($profile_data->phone1) . "', " .
                        "'" .       sqlesc($profile_data->phone2) . "', " .
                        "'" .       sqlesc($profile_data->mobile) . "', " .
                        "'" .       sqlesc($profile_data->fax) . "', " .
                        "'" .       sqlesc($profile_data->email) . "', " .
                        "'" .       sqlesc($profile_data->url) . "', " .
                        "'" .       sqlesc($profile_data->account_number) . "', " .
                        "'" .       sqlesc($profile_data->financial_institution) . "', " .
                        "'" .       sqlesc($profile_data->bankcode) . "', " .
                        "'" .       sqlesc($profile_data->account_holder) . "', " .
                        "'" .       sqlesc($profile_data->credit_card_number) . "', " .
                        "'" .       sqlesc($profile_data->credit_institute) . "', " .
                        "'" .       sqlesc($profile_data->credit_card_holder) . "', " .
                        "'" .       sqlesc($profile_data->credit_card_validity) . "', " .
                        "'" .       sqlesc($profile_data->comment) . "')"))
        {
            $profile_data->id = mysql_insert_id();

            $column = [CMS_DB_PROFILE_CUSTOM_INDEX];
            $insert = [sqlesc($profile_data->id)];

            for ($i = 1; $i < 21; $i++)
            {
                $column[] = CMS_DB_PROFILE_CUSTOM_FIELD . $i;
                $insert[] = sqlesc($profile_data->{"field$i"});
            };

            $query = "INSERT INTO " . CMS_DB_PROFILE_CUSTOM . " " .
                     "(" .            implode(", ",   $column) . ") " .
                     "VALUES ('" .    implode("', '", $insert) . "')";

            if (mysql_query($query)) return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function set(&$profile_data)
    {
        if (
             (! $this->enabled)
             ||
             (! $this->operator)
           )
            return FALSE;

        $profile_data->__construct();
        $index = sqlesc($profile_data->id);

        if ($flag = nstre($profile_data->password))
            $profile_data->password = hash64($profile_data->password);

        if (mysql_query("UPDATE " . CMS_DB_PROFILE . " " .
                        "SET " .    CMS_DB_PROFILE_TIME_UPDATED .          "='" . sqlesc(time()) . "', " .
                                    CMS_DB_PROFILE_CODE .                  "='" . sqlesc($profile_data->code) . "', " .
                                    CMS_DB_PROFILE_USER .                  "='" . sqlesc($profile_data->user) . "', " .
                        ($flag ?    CMS_DB_PROFILE_PASSWORD .              "='" . sqlesc($profile_data->password) . "', " : "") .
                                    CMS_DB_PROFILE_SUPERUSER .             "='" . sqlesc($profile_data->superuser) . "', " .
                                    CMS_DB_PROFILE_ENABLED .               "='" . sqlesc((bool)$profile_data->enabled) . "', " .
                                    CMS_DB_PROFILE_COMPANY .               "='" . sqlesc($profile_data->company) . "', " .
                                    CMS_DB_PROFILE_PRENAME .               "='" . sqlesc($profile_data->prename) . "', " .
                                    CMS_DB_PROFILE_SURNAME .               "='" . sqlesc($profile_data->surname) . "', " .
                                    CMS_DB_PROFILE_STREET .                "='" . sqlesc($profile_data->street) . "', " .
                                    CMS_DB_PROFILE_ZIPCODE .               "='" . sqlesc($profile_data->zipcode) . "', " .
                                    CMS_DB_PROFILE_CITY .                  "='" . sqlesc($profile_data->city) . "', " .
                                    CMS_DB_PROFILE_COUNTRY .               "='" . sqlesc($profile_data->country) . "', " .
                                    CMS_DB_PROFILE_PHONE_1 .               "='" . sqlesc($profile_data->phone1) . "', " .
                                    CMS_DB_PROFILE_PHONE_2 .               "='" . sqlesc($profile_data->phone2) . "', " .
                                    CMS_DB_PROFILE_MOBILE .                "='" . sqlesc($profile_data->mobile) . "', " .
                                    CMS_DB_PROFILE_FAX .                   "='" . sqlesc($profile_data->fax) . "', " .
                                    CMS_DB_PROFILE_EMAIL .                 "='" . sqlesc($profile_data->email) . "', " .
                                    CMS_DB_PROFILE_URL .                   "='" . sqlesc($profile_data->url) . "', " .
                                    CMS_DB_PROFILE_ACCOUNT_NUMBER .        "='" . sqlesc($profile_data->account_number) . "', " .
                                    CMS_DB_PROFILE_FINANCIAL_INSTITUTION . "='" . sqlesc($profile_data->financial_institution) . "', " .
                                    CMS_DB_PROFILE_BANKCODE .              "='" . sqlesc($profile_data->bankcode) . "', " .
                                    CMS_DB_PROFILE_ACCOUNT_HOLDER .        "='" . sqlesc($profile_data->account_holder) . "', " .
                                    CMS_DB_PROFILE_CREDIT_CARD_NUMBER .    "='" . sqlesc($profile_data->credit_card_number) . "', " .
                                    CMS_DB_PROFILE_CREDIT_INSTITUTE .      "='" . sqlesc($profile_data->credit_institute) . "', " .
                                    CMS_DB_PROFILE_CREDIT_CARD_HOLDER .    "='" . sqlesc($profile_data->credit_card_holder) . "', " .
                                    CMS_DB_PROFILE_CREDIT_CARD_VALIDITY .  "='" . sqlesc($profile_data->credit_card_validity) . "', " .
                                    CMS_DB_PROFILE_COMMENT .               "='" . sqlesc($profile_data->comment) . "' " .
                        "WHERE " .  CMS_DB_PROFILE_INDEX .                 "='$index' " .
                        "LIMIT 1"))
        {
            $column = [CMS_DB_PROFILE_CUSTOM_INDEX];
            $insert = [$index];
            $update = [];

            for ($i = 1; $i < 21; $i++)
            {
                $column[] = CMS_DB_PROFILE_CUSTOM_FIELD . $i;
                $insert[] = sqlesc($profile_data->{"field$i"});
                $update[] = CMS_DB_PROFILE_CUSTOM_FIELD . $i .
                            "=VALUES(" . CMS_DB_PROFILE_CUSTOM_FIELD . $i . ")";
            };

            $query = "INSERT INTO " .             CMS_DB_PROFILE_CUSTOM . " " .
                     "(" .                        implode(", ",   $column) . ") " .
                     "VALUES ('" .                implode("', '", $insert) . "') " .
                     "ON DUPLICATE KEY UPDATE " . implode(", ",   $update);

            if (mysql_query($query))
            {
                //set credential
                if ($flag)
                {
                    $password = hash64(cms_cache("cms.salt_password") . $profile_data->password);
                    cms_set_cookie(["cms_password" => $password]);
                };

                return TRUE;
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function get($index,
                 $index_field = CMS_DB_PROFILE_INDEX)
    {
        if (! $this->enabled) return FALSE;

        if (
             ($result = mysql_query("SELECT " .    CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_INDEX . ", " .                 //0
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_TIME_CREATED . ", " .          //1
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_TIME_UPDATED . ", " .          //2
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_CODE . ", " .                  //3
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_USER . ", " .                  //4
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_SUPERUSER . ", " .             //5
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_ENABLED . ", " .               //6
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_COMPANY . ", " .               //7
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_PRENAME . ", " .               //8
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_SURNAME . ", " .               //9
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_STREET . ", " .                //10
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_ZIPCODE . ", " .               //11
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_CITY . ", " .                  //12
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_COUNTRY . ", " .               //13
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_PHONE_1 . ", " .               //14
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_PHONE_2 . ", " .               //15
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_MOBILE . ", " .                //16
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_FAX . ", " .                   //17
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_EMAIL . ", " .                 //18
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_URL . ", " .                   //19
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_ACCOUNT_NUMBER . ", " .        //20
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_FINANCIAL_INSTITUTION . ", " . //21
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_BANKCODE . ", " .              //22
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_ACCOUNT_HOLDER . ", " .        //23
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_CREDIT_CARD_NUMBER . ", " .    //24
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_CREDIT_INSTITUTE . ", " .      //25
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_CREDIT_CARD_HOLDER . ", " .    //26
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_CREDIT_CARD_VALIDITY . ", " .  //27
                                                   CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_COMMENT . ", " .               //28
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "1, " .         //29
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "2, " .         //30
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "3, " .         //31
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "4, " .         //32
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "5, " .         //33
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "6, " .         //34
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "7, " .         //35
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "8, " .         //36
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "9, " .         //37
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "10, " .        //38
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "11, " .        //39
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "12, " .        //40
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "13, " .        //41
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "14, " .        //42
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "15, " .        //43
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "16, " .        //44
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "17, " .        //45
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "18, " .        //46
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "19, " .        //47
                                                   CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_FIELD . "20 " .         //48
                                    "FROM " .      CMS_DB_PROFILE . " " .
                                    "LEFT JOIN " . CMS_DB_PROFILE_CUSTOM . " " .
                                    "ON " .        CMS_DB_PROFILE .        "." . CMS_DB_PROFILE_INDEX .
                                    "=" .          CMS_DB_PROFILE_CUSTOM . "." . CMS_DB_PROFILE_CUSTOM_INDEX . " " .
                                    "WHERE " .     CMS_DB_PROFILE        . "." . sqlesc($index_field) .
                                    "='" .         sqlesc($index) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_row($result))
           )
        {
            $profile_data                        = new profile_data();
            $profile_data->id                    = $resultrow[0];
            $profile_data->time_created          = $resultrow[1];
            $profile_data->time_updated          = $resultrow[2];
            $profile_data->code                  = $resultrow[3];
            $profile_data->user                  = $resultrow[4];
            $profile_data->superuser             = $resultrow[5];
            $profile_data->enabled               = $resultrow[6];
            $profile_data->company               = $resultrow[7];
            $profile_data->prename               = $resultrow[8];
            $profile_data->surname               = $resultrow[9];
            $profile_data->street                = $resultrow[10];
            $profile_data->zipcode               = $resultrow[11];
            $profile_data->city                  = $resultrow[12];
            $profile_data->country               = $resultrow[13];
            $profile_data->phone1                = $resultrow[14];
            $profile_data->phone2                = $resultrow[15];
            $profile_data->mobile                = $resultrow[16];
            $profile_data->fax                   = $resultrow[17];
            $profile_data->email                 = $resultrow[18];
            $profile_data->url                   = $resultrow[19];
            $profile_data->account_number        = $resultrow[20];
            $profile_data->financial_institution = $resultrow[21];
            $profile_data->bankcode              = $resultrow[22];
            $profile_data->account_holder        = $resultrow[23];
            $profile_data->credit_card_number    = $resultrow[24];
            $profile_data->credit_institute      = $resultrow[25];
            $profile_data->credit_card_holder    = $resultrow[26];
            $profile_data->credit_card_validity  = $resultrow[27];
            $profile_data->comment               = $resultrow[28];

            for ($i = 1; $i <= 20; $i++)
            {
                $field = "field" . $i;
                $profile_data->$field = $resultrow[$i + 28];
            };

            $profile_data->__construct();
            return $profile_data;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function del($index)
    {
        if (
             (! $this->enabled)
             ||
             (! $this->operator)
           )
            return FALSE;

        $index = sqlesc($index);

        if (
             mysql_query("DELETE FROM " . CMS_DB_PROFILE . " " .
                         "WHERE " .       CMS_DB_PROFILE_INDEX .        "='$index' " .
                         "LIMIT 1")
             &&
             mysql_query("DELETE FROM " . CMS_DB_PROFILE_CUSTOM . " " .
                         "WHERE " .       CMS_DB_PROFILE_CUSTOM_INDEX . "='$index' " .
                         "LIMIT 1")
           )
            return TRUE;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function get_permission($user)
    {
        if (! $this->enabled) return FALSE;

        static $buffer = NULL;

        if (isset($buffer[$user])) return $buffer[$user];

        if (
             ($result = mysql_query("SELECT " . CMS_DB_PROFILE_INDEX . ", " .
                                                CMS_DB_PROFILE_PASSWORD . ", " .
                                                CMS_DB_PROFILE_SUPERUSER . ", " .
                                                CMS_DB_PROFILE_PRENAME . ", " .
                                                CMS_DB_PROFILE_SURNAME . " " .
                                    "FROM " .   CMS_DB_PROFILE . " " .
                                    "WHERE " .  CMS_DB_PROFILE_USER .    "='" . sqlesc($user) . "' " .
                                    "AND " .    CMS_DB_PROFILE_ENABLED . "='1' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_row($result))
           )
        {
            $array         = ["index"     => $resultrow[0],
                              "password"  => $resultrow[1],
                              "superuser" => $resultrow[2],
                              "name"      => trim($resultrow[3] . " " . $resultrow[4])];
            $buffer[$user] = $array;
            return $array;
        };

        $buffer[$user] = FALSE;
        return FALSE;
    }
};