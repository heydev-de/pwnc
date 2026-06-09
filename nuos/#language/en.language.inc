<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   ENGLISH LANGUAGE PACKAGE
//==================================================================================================

//language
//..................................................................................................
define("CMS_LANGUAGE_SEPARATOR",        chr(31)); //ascii unit separator

//error
//..................................................................................................
define("CMS_MSG_ERROR",                 "<strong>Error</strong><br>");
define("CMS_MSG_DONE",                  "<strong>OK</strong><br>");
define("CMS_MSG_UNAVAILABLE",           "The service is not available.");

//common
//..................................................................................................
define("CMS_L_USER_ANONYMOUS",          "Guest");
define("CMS_L_USER_UNKNOWN",            "Unknown");
define("CMS_L_NOSCRIPT",                "This application is not available or offers limited functionality " .
                                        "because JavaScript is disabled or not supported by your web software.");
define("CMS_L_NOCOOKIE",                "This application is not available or offers limited functionality " .
                                        "because cookies are disabled or not supported by your web software.");
define("CMS_L_NAME",                    "Name");
define("CMS_L_UNKNOWN",                 "Unknown");
define("CMS_L_PASSWORD",                "Password");
define("CMS_L_COLOR",                   "Color");
define("CMS_L_DESCRIPTION",             "Description");
define("CMS_L_EMAIL",                   "Email");
define("CMS_L_IMAGE",                   "Image");
define("CMS_L_MISSING",                 "Missing");
define("CMS_L_URL",                     "URL");
define("CMS_L_LANGUAGE_DEFAULT",        "default");
define("CMS_L_DATE_FORMAT",             "Y-m-d");
define("CMS_L_TIME_FORMAT",             "H:i:s");
define("CMS_L_DATETIME_FORMAT",         "Y-m-d, H:i:s");
define("CMS_L_SHORT_TIME_FORMAT",       "H:i");
define("CMS_L_SHORT_DATETIME_FORMAT",   "Y-m-d, H:i");
define("CMS_L_DECIMAL_SEPARATOR",       ".");
define("CMS_L_THOUSAND_SEPARATOR",      ",");

define("CMS_L_SUNDAY",                  "Sunday");
define("CMS_L_MONDAY",                  "Monday");
define("CMS_L_TUESDAY",                 "Tuesday");
define("CMS_L_WEDNESDAY",               "Wednesday");
define("CMS_L_THURSDAY",                "Thursday");
define("CMS_L_FRIDAY",                  "Friday");
define("CMS_L_SATURDAY",                "Saturday");

define("CMS_L_SUNDAY_ABBR",             "Sun");
define("CMS_L_MONDAY_ABBR",             "Mon");
define("CMS_L_TUESDAY_ABBR",            "Tue");
define("CMS_L_WEDNESDAY_ABBR",          "Wed");
define("CMS_L_THURSDAY_ABBR",           "Thu");
define("CMS_L_FRIDAY_ABBR",             "Fri");
define("CMS_L_SATURDAY_ABBR",           "Sat");

define("CMS_L_JANUARY",                 "January");
define("CMS_L_FEBRUARY",                "February");
define("CMS_L_MARCH",                   "March");
define("CMS_L_APRIL",                   "April");
define("CMS_L_MAY",                     "May");
define("CMS_L_JUNE",                    "June");
define("CMS_L_JULY",                    "July");
define("CMS_L_AUGUST",                  "August");
define("CMS_L_SEPTEMBER",               "September");
define("CMS_L_OCTOBER",                 "October");
define("CMS_L_NOVEMBER",                "November");
define("CMS_L_DECEMBER",                "December");

define("CMS_L_JANUARY_ABBR",            "Jan");
define("CMS_L_FEBRUARY_ABBR",           "Feb");
define("CMS_L_MARCH_ABBR",              "Mar");
define("CMS_L_APRIL_ABBR",              "Apr");
define("CMS_L_MAY_ABBR",                "May");
define("CMS_L_JUNE_ABBR",               "Jun");
define("CMS_L_JULY_ABBR",               "Jul");
define("CMS_L_AUGUST_ABBR",             "Aug");
define("CMS_L_SEPTEMBER_ABBR",          "Sep");
define("CMS_L_OCTOBER_ABBR",            "Oct");
define("CMS_L_NOVEMBER_ABBR",           "Nov");
define("CMS_L_DECEMBER_ABBR",           "Dec");

define("CMS_L_ACCESS",                  "Access");
define("CMS_L_READ",                    "Read");
define("CMS_L_WRITE",                   "Write");
define("CMS_L_OPERATOR",                "Operator");

define("CMS_L_ALL",                     "All");
define("CMS_L_INVERT",                  "Invert");
define("CMS_L_NONE",                    "None");

//common commands
//..................................................................................................
define("CMS_L_COMMAND_ADD",             "Create");
define("CMS_L_COMMAND_APPEND",          "Append");
define("CMS_L_COMMAND_BOTTOM",          "Bottom");
define("CMS_L_COMMAND_BUFFER",          "Buffer");
define("CMS_L_COMMAND_CANCEL",          "Cancel");
define("CMS_L_COMMAND_CLOSE",           "Close");
define("CMS_L_COMMAND_CONFIRM",         "OK");
define("CMS_L_COMMAND_CONNECT",         "Connect");
define("CMS_L_COMMAND_COPY",            "Copy");
define("CMS_L_COMMAND_CUT",             "Move");
define("CMS_L_COMMAND_DELETE",          "Delete");
define("CMS_L_COMMAND_DELETE_SELECTED", "Delete marked");
define("CMS_L_COMMAND_EDIT",            "Edit");
define("CMS_L_COMMAND_END",             "End");
define("CMS_L_COMMAND_EXPORT",          "Export");
define("CMS_L_COMMAND_INSERT",          "Insert");
define("CMS_L_COMMAND_NEXT",            "Next");
define("CMS_L_COMMAND_PASTE",           "Paste");
define("CMS_L_COMMAND_PREVIOUS",        "Previous");
define("CMS_L_COMMAND_PRINT",           "Print");
define("CMS_L_COMMAND_REFRESH",         "Refresh");
define("CMS_L_COMMAND_SAVE",            "Save");
define("CMS_L_COMMAND_SELECT",          "Select");
define("CMS_L_COMMAND_SHOW",            "Show");
define("CMS_L_COMMAND_START",           "Start");
define("CMS_L_COMMAND_TOP",             "Top");

//==================================================================================================
//   SYSTEM
//==================================================================================================

//common
//..................................................................................................
define("CMS_L_COMMON_001", "SYSTEM MESSAGE");
define("CMS_L_COMMON_002", "%u sec ago");
define("CMS_L_COMMON_003", "%u min ago");
define("CMS_L_COMMON_004", "Yesterday, %s");
define("CMS_L_COMMON_005", "%s, %s");
define("CMS_L_COMMON_006", "%s, %u days ago, %s");
define("CMS_L_COMMON_007", "%u %s, %s");
define("CMS_L_COMMON_008", "Yes");
define("CMS_L_COMMON_009", "No");
define("CMS_L_COMMON_010", "Clearance");
define("CMS_L_COMMON_011", "Inserts");
define("CMS_L_COMMON_012", "in %u sec");
define("CMS_L_COMMON_013", "in %u min");
define("CMS_L_COMMON_014", "Tomorrow, %s");
define("CMS_L_COMMON_015", "%u hrs, %u min ago");
define("CMS_L_COMMON_016", "%s, in %u days, %s");
define("CMS_L_COMMON_017", "in %u hrs, %u min");
define("CMS_L_COMMON_018", "%u hrs ago");
define("CMS_L_COMMON_019", "in %u hrs");
define("CMS_L_COMMON_020", "Yesterday");
define("CMS_L_COMMON_021", "Tomorrow");
define("CMS_L_COMMON_022", "%s");
define("CMS_L_COMMON_023", "%s, in %u days");
define("CMS_L_COMMON_024", "%s, %u days ago");
define("CMS_L_COMMON_025", "%u %s");
define("CMS_L_COMMON_026", "Today");
define("CMS_L_COMMON_027", "Now");
define("CMS_L_COMMON_028", "Preview");
define("CMS_L_COMMON_029", "Preview not available. " .
                           "IFRAMEs are disabled or not supported by your web software.");

//content_pool
//..................................................................................................
define("CMS_L_CONTENT_POOL_001", "New entry");

//core
//..................................................................................................
define("CMS_L_CORE_001", "%s has logged in.");
define("CMS_L_CORE_002", "%s has logged out.");
define("CMS_L_CORE_003", "%s enters group %s.");
define("CMS_L_CORE_004", "%s leaves group %s.");
define("CMS_L_CORE_005", "The group %s has been created.");
define("CMS_L_CORE_006", "The settings of group %s have been saved.");
define("CMS_L_CORE_007", "The group %s has been closed.");
define("CMS_L_CORE_008", "The profile has been saved.");
define("CMS_L_CORE_009", "%s in group %s is now %s.");
define("CMS_L_CORE_010", "group admin");
define("CMS_L_CORE_011", "moderator");
define("CMS_L_CORE_012", "absent");
define("CMS_L_CORE_013", "mute");
define("CMS_L_CORE_014", "banned");
define("CMS_L_CORE_015", "monitor");
define("CMS_L_CORE_016", "%s in group %s is no longer %s.");
define("CMS_L_CORE_017", "invisible");

//core_control
//..................................................................................................
define("CMS_L_CORE_CONTROL_001", "Groups");
define("CMS_L_CORE_CONTROL_002", "Participants");
define("CMS_L_CORE_CONTROL_003", "No settings available.");
define("CMS_L_CORE_CONTROL_004", "Group admin");
define("CMS_L_CORE_CONTROL_005", "Moderator");
define("CMS_L_CORE_CONTROL_006", "Member");
define("CMS_L_CORE_CONTROL_007", "Guest");
define("CMS_L_CORE_CONTROL_008", "Participant profile");
define("CMS_L_CORE_CONTROL_009", "Temporary groups");
define("CMS_L_CORE_CONTROL_010", "%u participants");
define("CMS_L_CORE_CONTROL_011", "Password required");
define("CMS_L_CORE_CONTROL_012", "Join");
define("CMS_L_CORE_CONTROL_013", "A password is required to enter this group.");
define("CMS_L_CORE_CONTROL_014", "You have been banned from the group %s.");
define("CMS_L_CORE_CONTROL_015", "Edit group");
define("CMS_L_CORE_CONTROL_016", "Close group");
define("CMS_L_CORE_CONTROL_017", "Create group");
define("CMS_L_CORE_CONTROL_018", "Permanent group");
define("CMS_L_CORE_CONTROL_019", "The selected group is not available.");
define("CMS_L_CORE_CONTROL_020", "Click on button to close the group %s.");
define("CMS_L_CORE_CONTROL_021", "Photo");
define("CMS_L_CORE_CONTROL_025", "Settings");
define("CMS_L_CORE_CONTROL_026", "Apply");
define("CMS_L_CORE_CONTROL_027", "Create");
define("CMS_L_CORE_CONTROL_028", "Monitor");
define("CMS_L_CORE_CONTROL_029", "Invisible");
define("CMS_L_CORE_CONTROL_030", "Absent");
define("CMS_L_CORE_CONTROL_031", "Mute");
define("CMS_L_CORE_CONTROL_032", "Banned");
define("CMS_L_CORE_CONTROL_035", "Private message");
define("CMS_L_CORE_CONTROL_036", "Recipient");
define("CMS_L_CORE_CONTROL_037", "Message text");
define("CMS_L_CORE_CONTROL_038", "Send");
define("CMS_L_CORE_CONTROL_040", "Administration");
define("CMS_L_CORE_CONTROL_041", "Profile");
define("CMS_L_CORE_CONTROL_042", "Profile text");
define("CMS_L_CORE_CONTROL_043", "Upload photo");
define("CMS_L_CORE_CONTROL_044", "GIF, JPG, PNG, WebP, max. %s");
define("CMS_L_CORE_CONTROL_045", "Select participant");
define("CMS_L_CORE_CONTROL_046", "The selected participant is not available.");
define("CMS_L_CORE_CONTROL_047", "Admin");
define("CMS_L_CORE_CONTROL_048", "Specify a name for the group to be created. " .
                                 "If the group already exists, a connection attempt will be made.");
define("CMS_L_CORE_CONTROL_049", "The selected file is too large.");
define("CMS_L_CORE_CONTROL_051", "Moderate group");
define("CMS_L_CORE_CONTROL_052", "Moderate participant");

//desktop
//..................................................................................................
define("CMS_L_DESKTOP_001", "In-box");
define("CMS_L_DESKTOP_002", "Appointments");
define("CMS_L_DESKTOP_003", "Contacts");

//directory
//..................................................................................................
define("CMS_L_DIRECTORY_002", "Start");

//flexview
//..................................................................................................
define("CMS_L_FLEXVIEW_001", "(Empty)");
define("CMS_L_FLEXVIEW_002", "Root");
define("CMS_L_FLEXVIEW_003", "Put here");

//forum
//..................................................................................................
define("CMS_L_FORUM_001", "Reply received");
define("CMS_L_FORUM_002", "You received a reply to your post \"%s\" at %s");

//ifc
//..................................................................................................
define("CMS_L_IFC_001", "This application is not active.");
define("CMS_L_IFC_002", "Close");
define("CMS_L_IFC_003", "Window is closing …");
define("CMS_L_IFC_004", "Expand/restore");
define("CMS_L_IFC_005", "Invalid value");
define("CMS_L_IFC_006", "Desktop");
define("CMS_L_IFC_007", "Log out");
define("CMS_L_IFC_008", "Website");
define("CMS_L_IFC_009", "Clearance");
define("CMS_L_IFC_010", "[Alt] + [W] : Clean up whitespace\n" .
                        "[Alt] + [F] : Auto-formatting");
define("CMS_L_IFC_011", "Update %s available.");
define("CMS_L_IFC_012", "Input");
define("CMS_L_IFC_013", "The usage of PWNC is and remains free of charge.<br>" .
                        "<a href=\"" . x(CMS_HOMEPAGE) . "/en/support.php\" " .
                           "onclick=\"window.open(this.href);return false;\">" .
                        "Please support the further development with a voluntary contribution." .
                        "</a>");

//log
//..................................................................................................
define("CMS_L_LOG_001", "No records found.");
define("CMS_L_LOG_002", "Sum: %s (100,0%%)");
define("CMS_L_LOG_003", "%u rows, %u columns; execution time: %01.10f seconds");
define("CMS_L_LOG_004", "%u rows edited; execution time: %01.10f seconds");

//media_type
//..................................................................................................
define("CMS_L_MEDIA_TYPE_001", "(Automatic)");
define("CMS_L_MEDIA_TYPE_002", "The resource <em>%s</em> is loading …");
define("CMS_L_MEDIA_TYPE_003", "The resource <a href=\"%s\"><em>%s</em></a> cannot be displayed. " .
                               "Additional software may be required for support.");

//permission
//..................................................................................................
define("CMS_L_PERMISSION_001", "Administrator");
define("CMS_L_PERMISSION_002", "Main administrative account");
define("CMS_L_PERMISSION_003", "Permissions for unidentified users");
define("CMS_L_PERMISSION_004", "User registration");
define("CMS_L_PERMISSION_005", "Default permissions for user registration");
define("CMS_L_PERMISSION_006", "Daemon");
define("CMS_L_PERMISSION_007", "Execution account for background tasks");

//pop
//..................................................................................................
define("CMS_L_POP_001", "The host %s is not available.");

//rss_parser
//..................................................................................................
define("CMS_L_RSS_PARSER_001", "Updated");
define("CMS_L_RSS_PARSER_002", "As of");
define("CMS_L_RSS_PARSER_003", "Generator");
define("CMS_L_RSS_PARSER_004", "Editor");
define("CMS_L_RSS_PARSER_005", "Web master");
define("CMS_L_RSS_PARSER_006", "Comments");
define("CMS_L_RSS_PARSER_007", "By");
define("CMS_L_RSS_PARSER_008", "Source");
define("CMS_L_RSS_PARSER_009", "Permanent link");
define("CMS_L_RSS_PARSER_010", "ID");

//smtp
//..................................................................................................
define("CMS_L_SMTP_001", "The host %s is not available.");
define("CMS_L_SMTP_002", "The host %s is invalid.");
define("CMS_L_SMTP_003", "The mail() function failed.");

//template
//..................................................................................................
define("CMS_L_TEMPLATE_001", "New entry");
define("CMS_L_TEMPLATE_002", "Preview");
define("CMS_L_TEMPLATE_003", "Plugin");
define("CMS_L_TEMPLATE_004", "Conditional blocks");
define("CMS_L_TEMPLATE_005", "Links");
define("CMS_L_TEMPLATE_006", "Plugins");
define("CMS_L_TEMPLATE_007", "Texts");
define("CMS_L_TEMPLATE_008", "Downloads");
define("CMS_L_TEMPLATE_009", "Images");
define("CMS_L_TEMPLATE_010", "Thumbnails");
define("CMS_L_TEMPLATE_011", "Templates");
define("CMS_L_TEMPLATE_012", "Full");
define("CMS_L_TEMPLATE_013", "Output");
define("CMS_L_TEMPLATE_015", "Layout");
define("CMS_L_TEMPLATE_016", "Content");
define("CMS_L_TEMPLATE_017", "Edit mode");
define("CMS_L_TEMPLATE_018", "Swap");
define("CMS_L_TEMPLATE_019", "Activate");
define("CMS_L_TEMPLATE_020", "Repeat");
define("CMS_L_TEMPLATE_021", "Media");
define("CMS_L_TEMPLATE_022", "Shift");
define("CMS_L_TEMPLATE_023", "Alternative blocks");
define("CMS_L_TEMPLATE_024", "Insert pushing up");
define("CMS_L_TEMPLATE_025", "Insert pushing down");
define("CMS_L_TEMPLATE_026", "Remove pulling up");
define("CMS_L_TEMPLATE_027", "Remove pulling down");
define("CMS_L_TEMPLATE_028", "Groups");
define("CMS_L_TEMPLATE_029", "Text");
define("CMS_L_TEMPLATE_030", "Download");
define("CMS_L_TEMPLATE_031", "Image");
define("CMS_L_TEMPLATE_032", "Thumbnail");
define("CMS_L_TEMPLATE_033", "Media");
define("CMS_L_TEMPLATE_034", "Template");
define("CMS_L_TEMPLATE_035", "Group");
define("CMS_L_TEMPLATE_036", "Repeat");
define("CMS_L_TEMPLATE_037", "Shift");
define("CMS_L_TEMPLATE_038", "Switch");
define("CMS_L_TEMPLATE_039", "Link");
define("CMS_L_TEMPLATE_040", "Create sync source");
define("CMS_L_TEMPLATE_041", "Link sync source");
define("CMS_L_TEMPLATE_042", "Synced");
define("CMS_L_TEMPLATE_043", "Value");
define("CMS_L_TEMPLATE_044", "Values");
define("CMS_L_TEMPLATE_045", "Double-click: Edit\n" .
                             "Drag &amp; drop: Move\n" .
                             "[Alt] + Drag &amp; drop: Duplicate");
define("CMS_L_TEMPLATE_046", "Click: Hide / Show\n" .
                             "[Alt] + click: Show exclusively");
define("CMS_L_TEMPLATE_047", "Debug");
define("CMS_L_TEMPLATE_048", "Functions");
define("CMS_L_TEMPLATE_049", "Modified");
define("CMS_L_TEMPLATE_050", "(Empty)");
define("CMS_L_TEMPLATE_051", "Active elements");
define("CMS_L_TEMPLATE_052", "Language");
define("CMS_L_TEMPLATE_053", "Off");
define("CMS_L_TEMPLATE_054", "On");
define("CMS_L_TEMPLATE_055", "Switches");
define("CMS_L_TEMPLATE_056", "Download %s");

//update
//..................................................................................................
define("CMS_L_UPDATE_001", "The folder %s could not be created.");
define("CMS_L_UPDATE_002", "The file %s could not be created.");
define("CMS_L_UPDATE_003", "The folder %s could not be opened.");
define("CMS_L_UPDATE_004", "The folder %s could not be added to the archive %s.");
define("CMS_L_UPDATE_005", "The file %s could not be added to the archive %s.");
define("CMS_L_UPDATE_006", "The file %s could not be properly closed.");
define("CMS_L_UPDATE_007", "The file %s could not be deleted.");
define("CMS_L_UPDATE_008", "The file %s could not be renamed to %s.");
define("CMS_L_UPDATE_009", "Backup is being created …");
define("CMS_L_UPDATE_010", "Backup completed successfully.");
define("CMS_L_UPDATE_011", "Backup completed with errors.");
define("CMS_L_UPDATE_012", "Update is being downloaded …");
define("CMS_L_UPDATE_013", "Could not connect to the update server.");
define("CMS_L_UPDATE_014", "Exclusive access to the file %s was not possible.");
define("CMS_L_UPDATE_015", "The update could not be downloaded.");
define("CMS_L_UPDATE_016", "Update downloaded successfully.");
define("CMS_L_UPDATE_017", "The file %s could not be opened.");
define("CMS_L_UPDATE_018", "The file %s could not be extracted from the archive %s.");
define("CMS_L_UPDATE_019", "Update is being installed …");
define("CMS_L_UPDATE_020", "The update could not be installed.");
define("CMS_L_UPDATE_021", "Update installed successfully.");
define("CMS_L_UPDATE_022", "The archive %s is empty.");
define("CMS_L_UPDATE_023", "Temporary data is being removed …");
define("CMS_L_UPDATE_024", "Temporary data has been removed.");
define("CMS_L_UPDATE_025", "Update completed.");
define("CMS_L_UPDATE_026", "Backup has been skipped.");
define("CMS_L_UPDATE_027", "Database is being exported …");
define("CMS_L_UPDATE_028", "Database could not be exported.");
define("CMS_L_UPDATE_029", "Database successfully exported.");
define("CMS_L_UPDATE_030", "Restoring the previous version has failed.");
define("CMS_L_UPDATE_031", "%s could not be restored.");
define("CMS_L_UPDATE_032", "The previous version is being restored …");
define("CMS_L_UPDATE_033", "The downloaded update is invalid.");

//==================================================================================================
//   MODULE
//==================================================================================================

//mod_blog
//..................................................................................................
define("CMS_L_MOD_BLOG_001", "More articles");
define("CMS_L_MOD_BLOG_002", "Write article");
define("CMS_L_MOD_BLOG_003", "Title");
define("CMS_L_MOD_BLOG_004", "Tags");
define("CMS_L_MOD_BLOG_005", "Save and view");
define("CMS_L_MOD_BLOG_006", "The article has been deleted.");
define("CMS_L_MOD_BLOG_007", "The article has been saved.");
define("CMS_L_MOD_BLOG_008", "The article has been created.");
define("CMS_L_MOD_BLOG_009", "Edit article");
define("CMS_L_MOD_BLOG_010", "Read on");
define("CMS_L_MOD_BLOG_011", "Overview");
define("CMS_L_MOD_BLOG_012", "The article could not be deleted.");
define("CMS_L_MOD_BLOG_013", "The article could not be saved.");
define("CMS_L_MOD_BLOG_014", "The article could not be created.");
define("CMS_L_MOD_BLOG_015", "%u articles");
define("CMS_L_MOD_BLOG_016", "Edit code");
define("CMS_L_MOD_BLOG_017", "The code has been saved.");
define("CMS_L_MOD_BLOG_018", "The code could not be saved.");
define("CMS_L_MOD_BLOG_019", "Archive");
define("CMS_L_MOD_BLOG_020", "Tags");
define("CMS_L_MOD_BLOG_021", "Show all");
define("CMS_L_MOD_BLOG_022", "Publication");
define("CMS_L_MOD_BLOG_023", "Now");
define("CMS_L_MOD_BLOG_024", "Disabled");
define("CMS_L_MOD_BLOG_025", "Day");
define("CMS_L_MOD_BLOG_026", "Month");
define("CMS_L_MOD_BLOG_027", "Year");
define("CMS_L_MOD_BLOG_028", "Hour");
define("CMS_L_MOD_BLOG_029", "Do you really want to remove this article?");
define("CMS_L_MOD_BLOG_030", "RSS feed");
define("CMS_L_MOD_BLOG_031", "Blog");
define("CMS_L_MOD_BLOG_032", "Placeholders");
define("CMS_L_MOD_BLOG_033", "by %s");
define("CMS_L_MOD_BLOG_034", "Author");
define("CMS_L_MOD_BLOG_035", "Username");
define("CMS_L_MOD_BLOG_036", "The requested article is not available (anymore).");
define("CMS_L_MOD_BLOG_037", "Highlight");
define("CMS_L_MOD_BLOG_038", "%u comments");
define("CMS_L_MOD_BLOG_039", "Minute");
define("CMS_L_MOD_BLOG_040", "Content");
define("CMS_L_MOD_BLOG_041", "HTML code");
define("CMS_L_MOD_BLOG_042", "Scheduled");
define("CMS_L_MOD_BLOG_043", "Article not found");
define("CMS_L_MOD_BLOG_044", "Error");
define("CMS_L_MOD_BLOG_045", "An error has occurred.");

//mod_chat
//..................................................................................................
define("CMS_L_MOD_CHAT_001", "<p>" .
                             "A connection is not possible at this time, " .
                             "because your ID is in use." .
                             "</p>" .
                             "<p>" .
                             "<strong>" .
                             "Please be patient …" .
                             "</strong>" .
                             "</p>" .
                             "<p>" .
                             "You will be automatically redirected to the login " .
                             "when your ID becomes clear again." .
                             "</p>");
define("CMS_L_MOD_CHAT_002", "Private message");
define("CMS_L_MOD_CHAT_003", "The name is already taken.");
define("CMS_L_MOD_CHAT_004", "You have been logged out.");
define("CMS_L_MOD_CHAT_005", "%s writes something.");
define("CMS_L_MOD_CHAT_006", "Play sounds");
define("CMS_L_MOD_CHAT_007", "Back");
define("CMS_L_MOD_CHAT_008", "Enter the name you would like to use.");
define("CMS_L_MOD_CHAT_009", "Set absent");
define("CMS_L_MOD_CHAT_011", "Focus on message");
define("CMS_L_MOD_CHAT_012", "Control");
define("CMS_L_MOD_CHAT_013", "Log in here if you have a user account.");
define("CMS_L_MOD_CHAT_014", "Chat");
define("CMS_L_MOD_CHAT_015", "Log in again");
define("CMS_L_MOD_CHAT_016", "Set absent on inactivity");
define("CMS_L_MOD_CHAT_017", "Log out");
define("CMS_L_MOD_CHAT_018", "Emojis");
define("CMS_L_MOD_CHAT_019", "<p>" .
                             "A connection is currently not available." .
                             "</p>" .
                             "<strong>" .
                             "Either there is a technical issue, " .
                             "or you have been excluded." .
                             "</strong>" .
                             "</p>");

//mod_comment
//..................................................................................................
define("CMS_L_MOD_COMMENT_001", "Please specify your name.");
define("CMS_L_MOD_COMMENT_002", "Please specify a valid email.");
define("CMS_L_MOD_COMMENT_003", "More comments");
define("CMS_L_MOD_COMMENT_004", "Your comment contains no text.");
define("CMS_L_MOD_COMMENT_005", "Please specify a name.");
define("CMS_L_MOD_COMMENT_006", "The comment contains no text.");
define("CMS_L_MOD_COMMENT_007", "Edit comment");
define("CMS_L_MOD_COMMENT_008", "Will not be published.");
define("CMS_L_MOD_COMMENT_009", "Homepage URL");
define("CMS_L_MOD_COMMENT_010", "Comment");
define("CMS_L_MOD_COMMENT_011", "Write comment");
define("CMS_L_MOD_COMMENT_012", "%u comment(s)");
define("CMS_L_MOD_COMMENT_013", "Do you want to delete the comment?");
define("CMS_L_MOD_COMMENT_015", "Disable");
define("CMS_L_MOD_COMMENT_016", "Enable");
define("CMS_L_MOD_COMMENT_017", "ID: %s");
define("CMS_L_MOD_COMMENT_018", "Homepage");
define("CMS_L_MOD_COMMENT_020", "Spam");
define("CMS_L_MOD_COMMENT_021", "Do you want to rate the comment as spam and delete it?");
define("CMS_L_MOD_COMMENT_022", "Your comment could not be saved.");
define("CMS_L_MOD_COMMENT_023", "The comment could not be saved.");
define("CMS_L_MOD_COMMENT_024", "The command could not be executed.");
define("CMS_L_MOD_COMMENT_025", "Rate positively");
define("CMS_L_MOD_COMMENT_026", "Rate negatively");
define("CMS_L_MOD_COMMENT_027", "New comment (Approval required)");
define("CMS_L_MOD_COMMENT_028", "New comment");
define("CMS_L_MOD_COMMENT_029", "You received a new comment at %s requiring approval:\n\n%s\n\n" .
                                "Click here to approve the comment:\n%s\n\n" .
                                "Click here to reject the comment:\n%s\n\n" .
                                "Click here if the comment resembles spam:\n%s");
define("CMS_L_MOD_COMMENT_030", "You received a new comment at %s :\n\n%s\n\n" .
                                "Click here to hide the comment:\n%s\n\n" .
                                "Click here to reject the comment:\n%s\n\n" .
                                "Click here if the comment resembles spam:\n%s");
define("CMS_L_MOD_COMMENT_031", "Your comment has been saved.");
define("CMS_L_MOD_COMMENT_032", "Your comment has been saved and will not appear " .
                                "before being approved by a moderator.");
define("CMS_L_MOD_COMMENT_033", "The changes have been saved.");
define("CMS_L_MOD_COMMENT_034", "Do you want to rate the comment negatively?");
define("CMS_L_MOD_COMMENT_035", "Do you want to rate the comment positively?");
define("CMS_L_MOD_COMMENT_036", "Rate negatively");
define("CMS_L_MOD_COMMENT_037", "Rate positively");
define("CMS_L_MOD_COMMENT_038", "Your comment has been identified as spam.");
define("CMS_L_MOD_COMMENT_039", "Spam probability");
define("CMS_L_MOD_COMMENT_040", "An identical comment has already been submitted.");
define("CMS_L_MOD_COMMENT_041", "Approval of your comment");
define("CMS_L_MOD_COMMENT_042", "Your comment at %s has just been approved.");
define("CMS_L_MOD_COMMENT_043", "CAPTCHA");
define("CMS_L_MOD_COMMENT_044", "Please enter the characters from the image.");
define("CMS_L_MOD_COMMENT_045", "The CAPTCHA code is invalid.");

//mod_content
//..................................................................................................
define("CMS_L_MOD_CONTENT_002", "The clipboard is empty.");
define("CMS_L_MOD_CONTENT_003", "Apply changes");
define("CMS_L_MOD_CONTENT_004", "Discard changes");
define("CMS_L_MOD_CONTENT_005", "Swap");
define("CMS_L_MOD_CONTENT_006", "Content analysis");
define("CMS_L_MOD_CONTENT_007", "Backend");
define("CMS_L_MOD_CONTENT_008", "Desktop");
define("CMS_L_MOD_CONTENT_009", "Repetitions");
define("CMS_L_MOD_CONTENT_010", "Position shift");
define("CMS_L_MOD_CONTENT_011", "Insert above");
define("CMS_L_MOD_CONTENT_012", "Insert below");
define("CMS_L_MOD_CONTENT_013", "The input is invalid.");
define("CMS_L_MOD_CONTENT_014", "Sync sources");
define("CMS_L_MOD_CONTENT_015", "Log out");
define("CMS_L_MOD_CONTENT_016", "Undo");
define("CMS_L_MOD_CONTENT_017", "Redo");
define("CMS_L_MOD_CONTENT_018", "Meta data");
define("CMS_L_MOD_CONTENT_020", "Debug info");
define("CMS_L_MOD_CONTENT_021", "Export page template");

//mod_desktop
//..................................................................................................
define("CMS_L_MOD_DESKTOP_001", "Create");
define("CMS_L_MOD_DESKTOP_002", "Upcoming appointments");
define("CMS_L_MOD_DESKTOP_003", "Link");
define("CMS_L_MOD_DESKTOP_004", "Memo");
define("CMS_L_MOD_DESKTOP_005", "Appointment");
define("CMS_L_MOD_DESKTOP_006", "Contact");
define("CMS_L_MOD_DESKTOP_007", "Folder");
define("CMS_L_MOD_DESKTOP_008", "Email");
define("CMS_L_MOD_DESKTOP_009", "Rename");
define("CMS_L_MOD_DESKTOP_010", "There are no recipients.");
define("CMS_L_MOD_DESKTOP_011", "Copy to …");
define("CMS_L_MOD_DESKTOP_012", "Recipient");
define("CMS_L_MOD_DESKTOP_013", "Legacy");
define("CMS_L_MOD_DESKTOP_014", "Current password");
define("CMS_L_MOD_DESKTOP_015", "New password");
define("CMS_L_MOD_DESKTOP_016", "Password confirmation");
define("CMS_L_MOD_DESKTOP_017", "Background image");
define("CMS_L_MOD_DESKTOP_018", "Favorites");
define("CMS_L_MOD_DESKTOP_019", "In-box");
define("CMS_L_MOD_DESKTOP_020", "Messenger");
define("CMS_L_MOD_DESKTOP_021", "Contacts");
define("CMS_L_MOD_DESKTOP_022", "Calendar");
define("CMS_L_MOD_DESKTOP_023", "Create %s");
define("CMS_L_MOD_DESKTOP_024", "Home");
define("CMS_L_MOD_DESKTOP_025", "Time zone");
define("CMS_L_MOD_DESKTOP_026", "Settings");
define("CMS_L_MOD_DESKTOP_027", "Change password");
define("CMS_L_MOD_DESKTOP_028", "Other settings");
define("CMS_L_MOD_DESKTOP_029", "Backend");
define("CMS_L_MOD_DESKTOP_030", "Website");
define("CMS_L_MOD_DESKTOP_031", "The current password is incorrect.");
define("CMS_L_MOD_DESKTOP_032", "New password and confirmation do not match.");

//mod_download
//..................................................................................................
define("CMS_L_MOD_DOWNLOAD_001", "Download");
define("CMS_L_MOD_DOWNLOAD_002", "<a href=\"%s\">Click here</a> " .
                                 "if the download does not start automatically after 3 seconds.");
define("CMS_L_MOD_DOWNLOAD_003", "You can <a href=\"javascript:window.close();\">close this window</a> " .
                                 "after downloading starts.");
define("CMS_L_MOD_DOWNLOAD_007", "The requested file ist not available.");

//mod_forum
//..................................................................................................
define("CMS_L_MOD_FORUM_001", "%s wrote on %s., %s at %s");
define("CMS_L_MOD_FORUM_002", "RE: ");
define("CMS_L_MOD_FORUM_003", "Create topic");
define("CMS_L_MOD_FORUM_004", "Search");
define("CMS_L_MOD_FORUM_005", "Search results");
define("CMS_L_MOD_FORUM_006", "Reply");
define("CMS_L_MOD_FORUM_008", "Your search returned no results.");
define("CMS_L_MOD_FORUM_009", "Notify by email about replies");
define("CMS_L_MOD_FORUM_010", "Write post");
define("CMS_L_MOD_FORUM_011", "Do you want to delete the post?");
define("CMS_L_MOD_FORUM_012", "Overview");
define("CMS_L_MOD_FORUM_013", "The post could not be deleted.");
define("CMS_L_MOD_FORUM_014", "Title");
define("CMS_L_MOD_FORUM_015", "by %s");
define("CMS_L_MOD_FORUM_016", "%u replies");
define("CMS_L_MOD_FORUM_017", "Please specify a title and content.");
define("CMS_L_MOD_FORUM_018", "The post could not be edited.");
define("CMS_L_MOD_FORUM_019", "The post has been deleted.");
define("CMS_L_MOD_FORUM_020", "The user does not exist or has not written any posts.");
define("CMS_L_MOD_FORUM_021", "The post \"%s\" ist being moved.");
define("CMS_L_MOD_FORUM_022", "The post has been created.");
define("CMS_L_MOD_FORUM_023", "The post could not be created.");
define("CMS_L_MOD_FORUM_024", "The post has been updated.");
define("CMS_L_MOD_FORUM_025", "The post could not be updated.");
define("CMS_L_MOD_FORUM_026", "The post has been moved.");
define("CMS_L_MOD_FORUM_027", "The post could not be moved.");
define("CMS_L_MOD_FORUM_028", "Content");
define("CMS_L_MOD_FORUM_029", "The requested post is not available (anymore).");
define("CMS_L_MOD_FORUM_030", "Post not found");

//mod_identification
//..................................................................................................
define("CMS_L_MOD_IDENTIFICATION_001", "You are currently logged in as %s (%s).");
define("CMS_L_MOD_IDENTIFICATION_002", "Username");
define("CMS_L_MOD_IDENTIFICATION_003", "Log out");
define("CMS_L_MOD_IDENTIFICATION_004", "Login");
define("CMS_L_MOD_IDENTIFICATION_005", "Log in");
define("CMS_L_MOD_IDENTIFICATION_006", "Forgot password?");
define("CMS_L_MOD_IDENTIFICATION_007", "You are being redirected to the login prompt. " .
                                       "<a href=\"%s\">Click here</a> " .
                                       "if the request is not processed by your web software.");
define("CMS_L_MOD_IDENTIFICATION_008", "Proceed as current user");
define("CMS_L_MOD_IDENTIFICATION_009", "Set new password");
define("CMS_L_MOD_IDENTIFICATION_010", "Please enter your username.");
define("CMS_L_MOD_IDENTIFICATION_011", "The specified user does not exist or has no valid email.");
define("CMS_L_MOD_IDENTIFICATION_012", "A message has been sent to the associated email, " .
                                       "by which you can set a new password.");
define("CMS_L_MOD_IDENTIFICATION_013", "The characters from the image are incorrect. " .
                                       "Please try again.");
define("CMS_L_MOD_IDENTIFICATION_014", "CAPTCHA");
define("CMS_L_MOD_IDENTIFICATION_015", "Please enter the characters from the image.");
define("CMS_L_MOD_IDENTIFICATION_016", "In order to overwrite the password belonging to the user %s " .
                                       "at %s, open the URL\n%s\nwithin one hour.\n\n" .
                                       "In case you have not requested a new password, " .
                                       "you can safely ignore this message.");
define("CMS_L_MOD_IDENTIFICATION_017", "The operation is invalid or expired.");
define("CMS_L_MOD_IDENTIFICATION_018", "A new password has been set.");
define("CMS_L_MOD_IDENTIFICATION_019", "The password has been sent to the associated email.");
define("CMS_L_MOD_IDENTIFICATION_020", "The new password could not be sent to the associated email.");
define("CMS_L_MOD_IDENTIFICATION_021", "A new password has been set for the user %s at %s.\n\n" .
                                       "\tKennwort:\t%s");
define("CMS_L_MOD_IDENTIFICATION_022", "Your new password");
define("CMS_L_MOD_IDENTIFICATION_023", "Legacy");
define("CMS_L_MOD_IDENTIFICATION_024", "Due to too many failed attempts, " .
                                       "you cannot log in for the next %u minutes.");
define("CMS_L_MOD_IDENTIFICATION_025", "You have %u remaining attempts.");

//mod_image
//..................................................................................................
define("CMS_L_MOD_IMAGE_001", "The image is not available.");

//mod_mailform
//..................................................................................................
define("CMS_L_MOD_MAILFORM_001", "Transmission confirmation");
define("CMS_L_MOD_MAILFORM_002", "The form has been transmitted.");
define("CMS_L_MOD_MAILFORM_003", "Send");
define("CMS_L_MOD_MAILFORM_005", "Field");
define("CMS_L_MOD_MAILFORM_006", "Value");
define("CMS_L_MOD_MAILFORM_008", "Please revise the marked input.");
define("CMS_L_MOD_MAILFORM_009", "CAPTCHA");
define("CMS_L_MOD_MAILFORM_010", "Please enter the characters from the image.");
define("CMS_L_MOD_MAILFORM_011", "Forms");
define("CMS_L_MOD_MAILFORM_012", "The form could not be transmitted.");

//mod_profile
//..................................................................................................
define("CMS_L_MOD_PROFILE_001", "(No value)");
define("CMS_L_MOD_PROFILE_002", "User credentials");
define("CMS_L_MOD_PROFILE_003", "Personal data");
define("CMS_L_MOD_PROFILE_004", "Contact information");
define("CMS_L_MOD_PROFILE_005", "Banking details");
define("CMS_L_MOD_PROFILE_006", "Credit card data");
define("CMS_L_MOD_PROFILE_007", "Miscellaneous");
define("CMS_L_MOD_PROFILE_008", "Identifier");
define("CMS_L_MOD_PROFILE_009", "Username");
define("CMS_L_MOD_PROFILE_010", "Custom field");
define("CMS_L_MOD_PROFILE_011", "Additional information");
define("CMS_L_MOD_PROFILE_012", "Organization");
define("CMS_L_MOD_PROFILE_013", "Forename");
define("CMS_L_MOD_PROFILE_014", "Surname");
define("CMS_L_MOD_PROFILE_015", "Street");
define("CMS_L_MOD_PROFILE_016", "Zip");
define("CMS_L_MOD_PROFILE_017", "City");
define("CMS_L_MOD_PROFILE_018", "Country");
define("CMS_L_MOD_PROFILE_019", "Phone");
define("CMS_L_MOD_PROFILE_020", "Phone 2");
define("CMS_L_MOD_PROFILE_021", "Cell phone");
define("CMS_L_MOD_PROFILE_022", "Fax");
define("CMS_L_MOD_PROFILE_023", "Account number");
define("CMS_L_MOD_PROFILE_024", "Bank");
define("CMS_L_MOD_PROFILE_025", "Bank code");
define("CMS_L_MOD_PROFILE_026", "Account holder");
define("CMS_L_MOD_PROFILE_027", "Credit card number");
define("CMS_L_MOD_PROFILE_028", "Credit institution");
define("CMS_L_MOD_PROFILE_029", "Credit card holder");
define("CMS_L_MOD_PROFILE_030", "Expiry");
define("CMS_L_MOD_PROFILE_031", "Remark");
define("CMS_L_MOD_PROFILE_032", "Password and password confirmation do not match.");
define("CMS_L_MOD_PROFILE_033", "The password must have at least 4 characters.");
define("CMS_L_MOD_PROFILE_034", "The changes have been saved.");
define("CMS_L_MOD_PROFILE_035", "Password confirmation");
define("CMS_L_MOD_PROFILE_036", "Please revise the marked input.");
define("CMS_L_MOD_PROFILE_037", "CAPTCHA");
define("CMS_L_MOD_PROFILE_038", "Please enter the characters from the image.");
define("CMS_L_MOD_PROFILE_039", "Registration");
define("CMS_L_MOD_PROFILE_040", "Create account");
define("CMS_L_MOD_PROFILE_041", "Registration confirmation");
define("CMS_L_MOD_PROFILE_042", "Thank you for registering at %s.\n\n" .
                                "\tUsername:\t%s\n\tPassword:\t%s\n\n" .
                                "Please confirm your registration within 24 hours " .
                                "by opening the URL\n%s");
define("CMS_L_MOD_PROFILE_043", "The registration process has been completed. " .
                                "You are about to receive a confirmation message by email " .
                                "that contains the code required to activate your account.");
define("CMS_L_MOD_PROFILE_044", "Please specify a valid email.");
define("CMS_L_MOD_PROFILE_045", "The username is already taken. " .
                                "Please choose an alternative.");
define("CMS_L_MOD_PROFILE_046", "The characters from the image are incorrect. " .
                                "Please try again.");
define("CMS_L_MOD_PROFILE_047", "The activation code is invalid or expired.");
define("CMS_L_MOD_PROFILE_048", "Your user account has been activated.");
define("CMS_L_MOD_PROFILE_049", "Leave this field empty if you do not want to change the password.");
define("CMS_L_MOD_PROFILE_051", "The field may not be empty.");
define("CMS_L_MOD_PROFILE_052", "An account using this email already exists.");
define("CMS_L_MOD_PROFILE_053", "Basic data");

//mod_profile_data
//..................................................................................................
define("CMS_L_MOD_PROFILE_DATA_001", "Phone");
define("CMS_L_MOD_PROFILE_DATA_002", "Phone 2");
define("CMS_L_MOD_PROFILE_DATA_003", "Cell phone");
define("CMS_L_MOD_PROFILE_DATA_004", "Fax");

//mod_rss
//..................................................................................................
define("CMS_L_MOD_RSS_001", "New");
define("CMS_L_MOD_RSS_002", "Update");

//mod_search
//..................................................................................................
define("CMS_L_MOD_SEARCH_001", "Search");
define("CMS_L_MOD_SEARCH_002", "Submit page");
define("CMS_L_MOD_SEARCH_004", "Submit");
define("CMS_L_MOD_SEARCH_005", "No results found for your search query.");
define("CMS_L_MOD_SEARCH_006", "There are no further results for your search query.");
define("CMS_L_MOD_SEARCH_007", "More results");
define("CMS_L_MOD_SEARCH_008", "%s | As of: %s | Score: %s");
define("CMS_L_MOD_SEARCH_010", "Search in %s pages");
define("CMS_L_MOD_SEARCH_011", " | %u similar page(s)");
define("CMS_L_MOD_SEARCH_012", "All languages");
define("CMS_L_MOD_SEARCH_013", "Search term");

//mod_security
//..................................................................................................
define("CMS_L_MOD_SECURITY_001", "Security validation");
define("CMS_L_MOD_SECURITY_002", "Validation for the following request has failed:<br>" .
                                 "<br>" .
                                 "<code>%s</code><br>" .
                                 "<br>" .
                                 "To prevent manipulation (CSRF), the action was stopped.");
define("CMS_L_MOD_SECURITY_003", "Unknown address");
define("CMS_L_MOD_SECURITY_004", "Go to home page");
define("CMS_L_MOD_SECURITY_005", "Ignore and continue");

//==================================================================================================
//   INTERFACE
//==================================================================================================

//ifc_content
//..................................................................................................
define("CMS_L_IFC_CONTENT",     "Editor");
define("CMS_L_IFC_CONTENT_001", "Execution time");
define("CMS_L_IFC_CONTENT_002", "Dispatch information");
define("CMS_L_IFC_CONTENT_003", "Sender");
define("CMS_L_IFC_CONTENT_004", "Date");
define("CMS_L_IFC_CONTENT_005", "Event type");
define("CMS_L_IFC_CONTENT_006", "Status");
define("CMS_L_IFC_CONTENT_007", "Type");
define("CMS_L_IFC_CONTENT_008", "Summary");
define("CMS_L_IFC_CONTENT_009", "Writer");
define("CMS_L_IFC_CONTENT_010", "Updated");
define("CMS_L_IFC_CONTENT_011", "Navigation");
define("CMS_L_IFC_CONTENT_012", "Keywords");
define("CMS_L_IFC_CONTENT_013", "Page template");
define("CMS_L_IFC_CONTENT_014", "Authorization");
define("CMS_L_IFC_CONTENT_015", "Reviever");
define("CMS_L_IFC_CONTENT_016", "Date");
define("CMS_L_IFC_CONTENT_017", "Memo");
define("CMS_L_IFC_CONTENT_018", "Publication");
define("CMS_L_IFC_CONTENT_019", "Publisher");
define("CMS_L_IFC_CONTENT_020", "Date");
define("CMS_L_IFC_CONTENT_021", "Create draft");
define("CMS_L_IFC_CONTENT_022", "My drafts");
define("CMS_L_IFC_CONTENT_023", "Settings");
define("CMS_L_IFC_CONTENT_024", "Publications");
define("CMS_L_IFC_CONTENT_025", "Show source");
define("CMS_L_IFC_CONTENT_026", "(Empty)");
define("CMS_L_IFC_CONTENT_027", "Title");
define("CMS_L_IFC_CONTENT_028", "Text");
define("CMS_L_IFC_CONTENT_029", "Authorize and lock (document)");
define("CMS_L_IFC_CONTENT_030", "Documents");
define("CMS_L_IFC_CONTENT_031", "Draft");
define("CMS_L_IFC_CONTENT_032", "Document");
define("CMS_L_IFC_CONTENT_033", "Publication");
define("CMS_L_IFC_CONTENT_034", "Original");
define("CMS_L_IFC_CONTENT_035", "Editable copy");
define("CMS_L_IFC_CONTENT_036", "Read-only copy");
define("CMS_L_IFC_CONTENT_037", "Noindex");
define("CMS_L_IFC_CONTENT_038", "Nofollow");
define("CMS_L_IFC_CONTENT_039", "Others' drafts");
define("CMS_L_IFC_CONTENT_040", "Publish");
define("CMS_L_IFC_CONTENT_041", "Link in navigation …");
define("CMS_L_IFC_CONTENT_042", "Publication");
define("CMS_L_IFC_CONTENT_043", "RSS channel");
define("CMS_L_IFC_CONTENT_044", "Function");
define("CMS_L_IFC_CONTENT_045", "There are no authorized recipients.");
define("CMS_L_IFC_CONTENT_046", "Forward");
define("CMS_L_IFC_CONTENT_047", "Author");
define("CMS_L_IFC_CONTENT_048", "Recipient");
define("CMS_L_IFC_CONTENT_049", "Create draft");
define("CMS_L_IFC_CONTENT_050", "Clear content cache");
define("CMS_L_IFC_CONTENT_051", "Create editable copy");
define("CMS_L_IFC_CONTENT_052", "Create read-only copy");
define("CMS_L_IFC_CONTENT_053", "Remove authorization and lock (draft)");
define("CMS_L_IFC_CONTENT_054", "Withdraw");
define("CMS_L_IFC_CONTENT_055", "In-box");
define("CMS_L_IFC_CONTENT_056", "Meta robots");
define("CMS_L_IFC_CONTENT_057", "Exclude");
define("CMS_L_IFC_CONTENT_058", "RSS channel of %s");
define("CMS_L_IFC_CONTENT_060", "Rows");
define("CMS_L_IFC_CONTENT_061", "RSS feed");
define("CMS_L_IFC_CONTENT_062", "Category"); //rss
define("CMS_L_IFC_CONTENT_063", "Rename category");
define("CMS_L_IFC_CONTENT_064", "Make marked default");
define("CMS_L_IFC_CONTENT_065", "ID");
define("CMS_L_IFC_CONTENT_066", "Statistics");
define("CMS_L_IFC_CONTENT_073", "Reset");
define("CMS_L_IFC_CONTENT_074", "Select default");
define("CMS_L_IFC_CONTENT_075", "Assign marked");
define("CMS_L_IFC_CONTENT_076", "Select assigned channels");
define("CMS_L_IFC_CONTENT_077", "Meta data");
define("CMS_L_IFC_CONTENT_078", "RSS");
define("CMS_L_IFC_CONTENT_079", "Subscribe to RSS channel");
define("CMS_L_IFC_CONTENT_080", "Source code");
define("CMS_L_IFC_CONTENT_084", "in process");
define("CMS_L_IFC_CONTENT_085", "Versions");
define("CMS_L_IFC_CONTENT_086", "Activate");
define("CMS_L_IFC_CONTENT_087", "Apply changes");
define("CMS_L_IFC_CONTENT_088", "Discard changes");
define("CMS_L_IFC_CONTENT_089", "Versions");
define("CMS_L_IFC_CONTENT_090", "Preview not available. " .
                                "IFRAMEs are disabled or not supported by your web software.");
define("CMS_L_IFC_CONTENT_091", "User box");
define("CMS_L_IFC_CONTENT_092", "Folder");
define("CMS_L_IFC_CONTENT_093", "Filter");
define("CMS_L_IFC_CONTENT_094", "All");
define("CMS_L_IFC_CONTENT_095", "Apply all changes");
define("CMS_L_IFC_CONTENT_096", "Textual content");
define("CMS_L_IFC_CONTENT_097", "Content analysis");
define("CMS_L_IFC_CONTENT_098", "Word");
define("CMS_L_IFC_CONTENT_099", "Occurences");
define("CMS_L_IFC_CONTENT_100", "Rate");
define("CMS_L_IFC_CONTENT_101", "Total");
define("CMS_L_IFC_CONTENT_102", "Word span");
define("CMS_L_IFC_CONTENT_103", "Single occurences are not displayed.");
define("CMS_L_IFC_CONTENT_104", "HTML");
define("CMS_L_IFC_CONTENT_108", "Content ID");
define("CMS_L_IFC_CONTENT_109", "Apply now");
define("CMS_L_IFC_CONTENT_110", "Save version");
define("CMS_L_IFC_CONTENT_111", "Website link");
define("CMS_L_IFC_CONTENT_116", "Publication details");
define("CMS_L_IFC_CONTENT_117", "Never apply");
define("CMS_L_IFC_CONTENT_118", "Additional");
define("CMS_L_IFC_CONTENT_119", "Overwrite selected entry");
define("CMS_L_IFC_CONTENT_120", "Schedule");
define("CMS_L_IFC_CONTENT_121", "Extra column");
define("CMS_L_IFC_CONTENT_122", "Activate version");
define("CMS_L_IFC_CONTENT_123", "Target path");
define("CMS_L_IFC_CONTENT_124", "Target path has not been specified or is invalid.");
define("CMS_L_IFC_CONTENT_125", "Version");
define("CMS_L_IFC_CONTENT_126", "Version is invalid.");
define("CMS_L_IFC_CONTENT_127", "Action");
define("CMS_L_IFC_CONTENT_128", "Publish without link");
define("CMS_L_IFC_CONTENT_129", "Category"); //pool
define("CMS_L_IFC_CONTENT_130", "Sync sources");
define("CMS_L_IFC_CONTENT_131", "Sync sources");
define("CMS_L_IFC_CONTENT_132", "Sync");
define("CMS_L_IFC_CONTENT_133", "Copy to clipboard");
define("CMS_L_IFC_CONTENT_134", "Set default for new pages");
define("CMS_L_IFC_CONTENT_135", "Version %u");
define("CMS_L_IFC_CONTENT_136", "Value");
define("CMS_L_IFC_CONTENT_137", "The selected page is not linked.");
define("CMS_L_IFC_CONTENT_138", "Info");
define("CMS_L_IFC_CONTENT_139", "XML sitemap");
define("CMS_L_IFC_CONTENT_140", "Settings");
define("CMS_L_IFC_CONTENT_141", "edited");
define("CMS_L_IFC_CONTENT_142", "Additional");
define("CMS_L_IFC_CONTENT_143", "All drafts");
define("CMS_L_IFC_CONTENT_144", "Label");
define("CMS_L_IFC_CONTENT_145", "Page ID");
define("CMS_L_IFC_CONTENT_146", "Debug info");

//ifc_database
//..................................................................................................
define("CMS_L_IFC_DATABASE",     "Database");
define("CMS_L_IFC_DATABASE_001", "Edit column");
define("CMS_L_IFC_DATABASE_002", "Create column");
define("CMS_L_IFC_DATABASE_003", "Data type");
define("CMS_L_IFC_DATABASE_004", "Type-specific attributes");
define("CMS_L_IFC_DATABASE_005", "Default value");
define("CMS_L_IFC_DATABASE_006", "Size");
define("CMS_L_IFC_DATABASE_007", "Attribute");
define("CMS_L_IFC_DATABASE_008", "Decimal places");
define("CMS_L_IFC_DATABASE_009", "Format");
define("CMS_L_IFC_DATABASE_010", "Length");
define("CMS_L_IFC_DATABASE_011", "Value list");
define("CMS_L_IFC_DATABASE_012", "Create index");
define("CMS_L_IFC_DATABASE_013", "Memory system");
define("CMS_L_IFC_DATABASE_014", "Edit table");
define("CMS_L_IFC_DATABASE_015", "Create table");
define("CMS_L_IFC_DATABASE_016", "Description");
define("CMS_L_IFC_DATABASE_017", "Data editor");
define("CMS_L_IFC_DATABASE_018", "Display");
define("CMS_L_IFC_DATABASE_021", "Rows");
define("CMS_L_IFC_DATABASE_022", "Characters");
define("CMS_L_IFC_DATABASE_023", "Filter");
define("CMS_L_IFC_DATABASE_024", "Contains");
define("CMS_L_IFC_DATABASE_025", "Sorting");
define("CMS_L_IFC_DATABASE_026", "%s Records");
define("CMS_L_IFC_DATABASE_027", "Export file name");
define("CMS_L_IFC_DATABASE_028", "Column separator");
define("CMS_L_IFC_DATABASE_029", "Comma (MS Excel)");
define("CMS_L_IFC_DATABASE_030", "Semicolon (MS Access)");
define("CMS_L_IFC_DATABASE_031", "Tab");
define("CMS_L_IFC_DATABASE_032", "Space");
define("CMS_L_IFC_DATABASE_033", "Value separator");
define("CMS_L_IFC_DATABASE_034", "\" (MS Excel, MS Access)");
define("CMS_L_IFC_DATABASE_035", "'");
define("CMS_L_IFC_DATABASE_036", "(None)");
define("CMS_L_IFC_DATABASE_037", "Export data");
define("CMS_L_IFC_DATABASE_038", "Import data");
define("CMS_L_IFC_DATABASE_039", "Import file");
define("CMS_L_IFC_DATABASE_040", "Connection not established.");
define("CMS_L_IFC_DATABASE_041", "MySQL server");
define("CMS_L_IFC_DATABASE_042", "MySQL database");
define("CMS_L_IFC_DATABASE_043", "User");
define("CMS_L_IFC_DATABASE_044", "Password");
define("CMS_L_IFC_DATABASE_045", "Add record");
define("CMS_L_IFC_DATABASE_046", "Ignore existing records");
define("CMS_L_IFC_DATABASE_047", "Download export file");
define("CMS_L_IFC_DATABASE_048", "CSV settings");
define("CMS_L_IFC_DATABASE_050", "Set up connection");
define("CMS_L_IFC_DATABASE_051", "SQL console");
define("CMS_L_IFC_DATABASE_052", "SQL command");
define("CMS_L_IFC_DATABASE_053", "Maintain database");
define("CMS_L_IFC_DATABASE_054", "Table");
define("CMS_L_IFC_DATABASE_056", "Record format");
define("CMS_L_IFC_DATABASE_057", "Number of records");
define("CMS_L_IFC_DATABASE_058", "Average record size");
define("CMS_L_IFC_DATABASE_059", "Data size");
define("CMS_L_IFC_DATABASE_060", "Maximum data size");
define("CMS_L_IFC_DATABASE_061", "Index size");
define("CMS_L_IFC_DATABASE_062", "Free allocated memory");
define("CMS_L_IFC_DATABASE_063", "Next automatic value");
define("CMS_L_IFC_DATABASE_064", "Created");
define("CMS_L_IFC_DATABASE_065", "Updated");
define("CMS_L_IFC_DATABASE_066", "Checked");
define("CMS_L_IFC_DATABASE_067", "NULL value");
define("CMS_L_IFC_DATABASE_068", "Index");
define("CMS_L_IFC_DATABASE_069", "Options");
define("CMS_L_IFC_DATABASE_070", "Unique values");
define("CMS_L_IFC_DATABASE_071", "Index position");
define("CMS_L_IFC_DATABASE_073", "Number of unique values");
define("CMS_L_IFC_DATABASE_074", "Use structure");
define("CMS_L_IFC_DATABASE_075", "Save structure");
define("CMS_L_IFC_DATABASE_077", "First line contains column names");
define("CMS_L_IFC_DATABASE_079", "(Empty)");
define("CMS_L_IFC_DATABASE_080", "Does not contain");
define("CMS_L_IFC_DATABASE_081", "Execute");
define("CMS_L_IFC_DATABASE_082", "Backup database");
define("CMS_L_IFC_DATABASE_083", "Restore database");
define("CMS_L_IFC_DATABASE_084", "Format");
define("CMS_L_IFC_DATABASE_085", "Compression");
define("CMS_L_IFC_DATABASE_086", "Version");
define("CMS_L_IFC_DATABASE_087", "Checksum");
define("CMS_L_IFC_DATABASE_088", "Collation");
define("CMS_L_IFC_DATABASE_089", "Authorization");
define("CMS_L_IFC_DATABASE_090", "(Default)");
define("CMS_L_IFC_DATABASE_091", "Index type");
define("CMS_L_IFC_DATABASE_092", "Included columns");
define("CMS_L_IFC_DATABASE_093", "Column");
define("CMS_L_IFC_DATABASE_094", "Value");
define("CMS_L_IFC_DATABASE_095", "Column assignment");

//ifc_directory
//..................................................................................................
define("CMS_L_IFC_DIRECTORY",     "Navigation");
define("CMS_L_IFC_DIRECTORY_001", "New link");
define("CMS_L_IFC_DIRECTORY_002", "Remove unused links");
define("CMS_L_IFC_DIRECTORY_003", "Step 1: Settings");
define("CMS_L_IFC_DIRECTORY_004", "Step 2: Position");
define("CMS_L_IFC_DIRECTORY_005", "Hidden");
define("CMS_L_IFC_DIRECTORY_006", "Drag here to delete");
define("CMS_L_IFC_DIRECTORY_007", "Settings");
define("CMS_L_IFC_DIRECTORY_008", "Link title");
define("CMS_L_IFC_DIRECTORY_009", "Active (Image/URL)");
define("CMS_L_IFC_DIRECTORY_010", "Hide empty");
define("CMS_L_IFC_DIRECTORY_011", "Graphics");
define("CMS_L_IFC_DIRECTORY_012", "Normal (Image/URL)");
define("CMS_L_IFC_DIRECTORY_013", "Hover (Image/URL)");
define("CMS_L_IFC_DIRECTORY_014", "Link");
define("CMS_L_IFC_DIRECTORY_015", "Set path");
define("CMS_L_IFC_DIRECTORY_016", "Delete automatically");
define("CMS_L_IFC_DIRECTORY_017", "New icon");
define("CMS_L_IFC_DIRECTORY_018", "Link description");
define("CMS_L_IFC_DIRECTORY_019", "Data");
define("CMS_L_IFC_DIRECTORY_021", "Canonical base URL");
define("CMS_L_IFC_DIRECTORY_022", "Canonical URL");
define("CMS_L_IFC_DIRECTORY_062", "Icons");
define("CMS_L_IFC_DIRECTORY_063", "Icon");
define("CMS_L_IFC_DIRECTORY_064", "Normal image");
define("CMS_L_IFC_DIRECTORY_065", "Active image");
define("CMS_L_IFC_DIRECTORY_072", "Sort alphabetically");

//ifc_download
//..................................................................................................
define("CMS_L_IFC_DOWNLOAD",     "Downloads");
define("CMS_L_IFC_DOWNLOAD_001", "Upload file");
define("CMS_L_IFC_DOWNLOAD_002", "File");
define("CMS_L_IFC_DOWNLOAD_003", "Category");
define("CMS_L_IFC_DOWNLOAD_004", "Preview not available. " .
                                 "IFRAMEs are disabled or not supported by your web software.");
define("CMS_L_IFC_DOWNLOAD_005", "File information");
define("CMS_L_IFC_DOWNLOAD_006", "Rename category");
define("CMS_L_IFC_DOWNLOAD_007", "Downloads");
define("CMS_L_IFC_DOWNLOAD_008", "Replace file");
define("CMS_L_IFC_DOWNLOAD_011", "Format");
define("CMS_L_IFC_DOWNLOAD_012", "Size");
define("CMS_L_IFC_DOWNLOAD_013", "Download");
define("CMS_L_IFC_DOWNLOAD_014", "Bulk upload");
define("CMS_L_IFC_DOWNLOAD_015", "Here you can select multiple files for upload.");

//ifc_easypage
//..................................................................................................
define("CMS_L_IFC_EASYPAGE",     "Web pages");
define("CMS_L_IFC_EASYPAGE_001", "Navigation");
define("CMS_L_IFC_EASYPAGE_002", "Settings");
define("CMS_L_IFC_EASYPAGE_003", "Step 1: Settings");
define("CMS_L_IFC_EASYPAGE_004", "Drag here to delete");
define("CMS_L_IFC_EASYPAGE_005", "Keywords");
define("CMS_L_IFC_EASYPAGE_006", "Step 2: Position");
define("CMS_L_IFC_EASYPAGE_007", "New web page");
define("CMS_L_IFC_EASYPAGE_008", "Here you can specify the web page data.");
define("CMS_L_IFC_EASYPAGE_009", "Link title");
define("CMS_L_IFC_EASYPAGE_010", "Select the page you want to edit.");
define("CMS_L_IFC_EASYPAGE_011", "Hidden");
define("CMS_L_IFC_EASYPAGE_013", "Page title");
define("CMS_L_IFC_EASYPAGE_014", "Meta data");
define("CMS_L_IFC_EASYPAGE_015", "Page template");
define("CMS_L_IFC_EASYPAGE_021", "Select the position for the new web page.");
define("CMS_L_IFC_EASYPAGE_022", "Here you can specify the title and design of the new web page.");

//ifc_filemanager
//..................................................................................................
define("CMS_L_IFC_FILEMANAGER",     "Files");
define("CMS_L_IFC_FILEMANAGER_001", "Upload");
define("CMS_L_IFC_FILEMANAGER_002", "File upload");
define("CMS_L_IFC_FILEMANAGER_003", "File contents");
define("CMS_L_IFC_FILEMANAGER_004", "File name");
define("CMS_L_IFC_FILEMANAGER_005", "Copy to …");
define("CMS_L_IFC_FILEMANAGER_006", "Create folder");
define("CMS_L_IFC_FILEMANAGER_007", "Create file");
define("CMS_L_IFC_FILEMANAGER_008", "Add link to Navigation");
define("CMS_L_IFC_FILEMANAGER_009", "Settings");
define("CMS_L_IFC_FILEMANAGER_010", "File permissions");
define("CMS_L_IFC_FILEMANAGER_011", "Owner");
define("CMS_L_IFC_FILEMANAGER_012", "Group");
define("CMS_L_IFC_FILEMANAGER_013", "Other");
define("CMS_L_IFC_FILEMANAGER_014", "Read");
define("CMS_L_IFC_FILEMANAGER_015", "Write");
define("CMS_L_IFC_FILEMANAGER_016", "Execute");
define("CMS_L_IFC_FILEMANAGER_017", "Write protection (Windows)");
define("CMS_L_IFC_FILEMANAGER_018", "Type");
define("CMS_L_IFC_FILEMANAGER_019", "Size");
define("CMS_L_IFC_FILEMANAGER_020", "Updated");
define("CMS_L_IFC_FILEMANAGER_021", "Accessed");
define("CMS_L_IFC_FILEMANAGER_022", "Available storage space");
define("CMS_L_IFC_FILEMANAGER_023", "Download time (10 Mbit/s)");
define("CMS_L_IFC_FILEMANAGER_024", "Design");
define("CMS_L_IFC_FILEMANAGER_025", "Data");
define("CMS_L_IFC_FILEMANAGER_026", "Database");
define("CMS_L_IFC_FILEMANAGER_027", "Drag files onto the above field to upload.");
define("CMS_L_IFC_FILEMANAGER_028", "Logs");
define("CMS_L_IFC_FILEMANAGER_029", "Download");
define("CMS_L_IFC_FILEMANAGER_030", "Move to …");
define("CMS_L_IFC_FILEMANAGER_031", "Libraries");
define("CMS_L_IFC_FILEMANAGER_032", "3rd party");
define("CMS_L_IFC_FILEMANAGER_033", "Transmit from …");
define("CMS_L_IFC_FILEMANAGER_034", "Localization");
define("CMS_L_IFC_FILEMANAGER_035", "Backup");
define("CMS_L_IFC_FILEMANAGER_036", "GZip");
define("CMS_L_IFC_FILEMANAGER_037", "Un-GZip");
define("CMS_L_IFC_FILEMANAGER_038", "Total storage space");
define("CMS_L_IFC_FILEMANAGER_039", "MIME type");
define("CMS_L_IFC_FILEMANAGER_040", "File info");

//ifc_image
//..................................................................................................
define("CMS_L_IFC_IMAGE",     "Images");
define("CMS_L_IFC_IMAGE_001", "Upload image");
define("CMS_L_IFC_IMAGE_002", "Image file");
define("CMS_L_IFC_IMAGE_003", "Configuration");
define("CMS_L_IFC_IMAGE_004", "Preview not available. " .
                              "IFRAMEs are disabled or not supported by your web software.");
define("CMS_L_IFC_IMAGE_005", "Process images in background");
define("CMS_L_IFC_IMAGE_006", "Preferred image format");
define("CMS_L_IFC_IMAGE_007", "Images");
define("CMS_L_IFC_IMAGE_008", "Replace image");
define("CMS_L_IFC_IMAGE_009", "Add URL");
define("CMS_L_IFC_IMAGE_010", "Rename category");
define("CMS_L_IFC_IMAGE_011", "Clear image cache");
define("CMS_L_IFC_IMAGE_012", "The source is external and cannot be replaced internally. " .
                              "Select <em>Edit</em> to change the source.");
define("CMS_L_IFC_IMAGE_013", "File name");
define("CMS_L_IFC_IMAGE_014", "Category");
define("CMS_L_IFC_IMAGE_015", "Additional Settings");
define("CMS_L_IFC_IMAGE_016", "Bulk upload");
define("CMS_L_IFC_IMAGE_017", "Here you can select multiple files for upload.");
define("CMS_L_IFC_IMAGE_018", "Maximum image resolution");

//ifc_insert
//..................................................................................................
define("CMS_L_IFC_INSERT",     "Inserts");
define("CMS_L_IFC_INSERT_002", "New insert");
define("CMS_L_IFC_INSERT_003", "Insert");
define("CMS_L_IFC_INSERT_004", "Inserts");
define("CMS_L_IFC_INSERT_005", "Settings");
define("CMS_L_IFC_INSERT_006", "HTML");
define("CMS_L_IFC_INSERT_007", "Content");
define("CMS_L_IFC_INSERT_008", "Position");

//ifc_language
//..................................................................................................
define("CMS_L_IFC_LANGUAGE",     "Languages");
define("CMS_L_IFC_LANGUAGE_004", "IETF code");
define("CMS_L_IFC_LANGUAGE_005", "Language name");
define("CMS_L_IFC_LANGUAGE_006", "Settings");
define("CMS_L_IFC_LANGUAGE_007", "Icon");
define("CMS_L_IFC_LANGUAGE_009", "Activate marked");
define("CMS_L_IFC_LANGUAGE_010", "New language");
define("CMS_L_IFC_LANGUAGE_011", "Select active");
define("CMS_L_IFC_LANGUAGE_012", "Stop words (whitespace-separated)");
define("CMS_L_IFC_LANGUAGE_013", "Default language");

//ifc_log
//..................................................................................................
define("CMS_L_IFC_LOG",     "Log");
define("CMS_L_IFC_LOG_001", "Data retention");
define("CMS_L_IFC_LOG_002", "Unlimited");
define("CMS_L_IFC_LOG_003", "3 years");
define("CMS_L_IFC_LOG_004", "2 years");
define("CMS_L_IFC_LOG_005", "1 year");
define("CMS_L_IFC_LOG_006", "6 months");
define("CMS_L_IFC_LOG_007", "3 months");
define("CMS_L_IFC_LOG_008", "1 month");
define("CMS_L_IFC_LOG_009", "2 weeks");
define("CMS_L_IFC_LOG_010", "1 week");
define("CMS_L_IFC_LOG_011", "3 days");
define("CMS_L_IFC_LOG_012", "2 days");
define("CMS_L_IFC_LOG_013", "1 day");
define("CMS_L_IFC_LOG_014", "Disabled");
define("CMS_L_IFC_LOG_015", "Anonymize IP");
define("CMS_L_IFC_LOG_016", "Don't store user data");
define("CMS_L_IFC_LOG_017", "Show more");
define("CMS_L_IFC_LOG_018", "Day");
define("CMS_L_IFC_LOG_019", "Week");
define("CMS_L_IFC_LOG_020", "Month");
define("CMS_L_IFC_LOG_021", "Quarter");
define("CMS_L_IFC_LOG_022", "Half-year");
define("CMS_L_IFC_LOG_023", "Year");
define("CMS_L_IFC_LOG_024", "2 years");
define("CMS_L_IFC_LOG_025", "3 years");
define("CMS_L_IFC_LOG_026", "4 years");
define("CMS_L_IFC_LOG_027", "5 years");
define("CMS_L_IFC_LOG_028", "Decade");
define("CMS_L_IFC_LOG_029", "User-ID");
define("CMS_L_IFC_LOG_030", "User");
define("CMS_L_IFC_LOG_031", "IP");
define("CMS_L_IFC_LOG_032", "Action");
define("CMS_L_IFC_LOG_033", "Path");
define("CMS_L_IFC_LOG_034", "URL");
define("CMS_L_IFC_LOG_035", "Ref. domain");
define("CMS_L_IFC_LOG_036", "Ref. URL");
define("CMS_L_IFC_LOG_037", "Client");
define("CMS_L_IFC_LOG_038", "User-agent");
define("CMS_L_IFC_LOG_039", "Language");
define("CMS_L_IFC_LOG_040", "Region");
define("CMS_L_IFC_LOG_041", "Contains");
define("CMS_L_IFC_LOG_042", "Not");
define("CMS_L_IFC_LOG_043", "Is");
define("CMS_L_IFC_LOG_044", "Settings");
define("CMS_L_IFC_LOG_045", "End date");
define("CMS_L_IFC_LOG_046", "Time range");
define("CMS_L_IFC_LOG_047", "Filter");
define("CMS_L_IFC_LOG_048", "Bots");
define("CMS_L_IFC_LOG_049", "Off");
define("CMS_L_IFC_LOG_050", "Only");
define("CMS_L_IFC_LOG_051", "All");
define("CMS_L_IFC_LOG_052", "Reset");
define("CMS_L_IFC_LOG_053", "Accesses");
define("CMS_L_IFC_LOG_054", "Graph");
define("CMS_L_IFC_LOG_055", "Referrers");
define("CMS_L_IFC_LOG_056", "Visited pages");
define("CMS_L_IFC_LOG_057", "Top actions");
define("CMS_L_IFC_LOG_058", "Clients");
define("CMS_L_IFC_LOG_059", "Preferred languages");
define("CMS_L_IFC_LOG_060", "Regions");
define("CMS_L_IFC_LOG_061", "Most active users");
define("CMS_L_IFC_LOG_062", "Activity by time of day");
define("CMS_L_IFC_LOG_063", "Pseudonym");
define("CMS_L_IFC_LOG_064", "Info");
define("CMS_L_IFC_LOG_065", "Status");
define("CMS_L_IFC_LOG_066", "Details");
define("CMS_L_IFC_LOG_067", "Date");
define("CMS_L_IFC_LOG_068", "Time");
define("CMS_L_IFC_LOG_069", "Referrer");
define("CMS_L_IFC_LOG_070", "Count");
define("CMS_L_IFC_LOG_071", "Mobile");
define("CMS_L_IFC_LOG_072", "Status");
define("CMS_L_IFC_LOG_073", "User (fixed)");
define("CMS_L_IFC_LOG_074", "User (provisional)");
define("CMS_L_IFC_LOG_075", "Bot (provisional)");
define("CMS_L_IFC_LOG_076", "Bot (limit exceeded)");
define("CMS_L_IFC_LOG_077", "Bot (fixed)");
define("CMS_L_IFC_LOG_078", "Bad bot");
define("CMS_L_IFC_LOG_079", "Bot detection and handling");
define("CMS_L_IFC_LOG_080", "Bot detection threshold");
define("CMS_L_IFC_LOG_081", "This is the number of accesses per 10 seconds " .
                            "that will trigger bot detection.");
define("CMS_L_IFC_LOG_082", "Bad bot detection threshold");
define("CMS_L_IFC_LOG_083", "This is the number of invalid accesses per 10 seconds " .
                            "that will trigger bad bot detection. " .
                            "WARNING: Too aggressive thresholds may misclassify search engine crawlers, " .
                            "leading to search engine exclusion if the countermeasure is active.");
define("CMS_L_IFC_LOG_084", "Countermeasure delay");
define("CMS_L_IFC_LOG_085", "This sets the delay after bad bot detection " .
                            "before the countermeasure takes effect. " .
                            "It prevents bots from learning and bypassing the detection threshold.");
define("CMS_L_IFC_LOG_086", "Enable bad bot countermeasure");
define("CMS_L_IFC_LOG_087", "Terminates bad bot requests without output, " .
                            "saves resources and prevents information leaks.");
define("CMS_L_IFC_LOG_088", "Privacy");
define("CMS_L_IFC_LOG_089", "%u accesses / 10 seconds");
define("CMS_L_IFC_LOG_090", "%u seconds");
define("CMS_L_IFC_LOG_091", "Error page");
define("CMS_L_IFC_LOG_092", "Bot status reset interval");
define("CMS_L_IFC_LOG_093", "This is the period after the last access " .
                            "after which the bot status is reset to &quot;Bot (provisional)&quot;, " .
                            "if the status is not fixed.");
define("CMS_L_IFC_LOG_094", "Origin");
define("CMS_L_IFC_LOG_095", "Loading …");
define("CMS_L_IFC_LOG_096", "Activity");
define("CMS_L_IFC_LOG_097", "Region");
define("CMS_L_IFC_LOG_098", "Technology");
define("CMS_L_IFC_LOG_099", "Identity");
define("CMS_L_IFC_LOG_100", "Content");
define("CMS_L_IFC_LOG_101", "Time");
define("CMS_L_IFC_LOG_102", "Language");
define("CMS_L_IFC_LOG_103", "Bot data retention");
define("CMS_L_IFC_LOG_104", "This is the time period for which bot access data is retained.");

//ifc_mailform
//..................................................................................................
define("CMS_L_IFC_MAILFORM",     "Forms");
define("CMS_L_IFC_MAILFORM_001", "New form");
define("CMS_L_IFC_MAILFORM_002", "Create element");
define("CMS_L_IFC_MAILFORM_003", "Type");
define("CMS_L_IFC_MAILFORM_004", "Options");
define("CMS_L_IFC_MAILFORM_005", "Choice");
define("CMS_L_IFC_MAILFORM_006", "Choice list");
define("CMS_L_IFC_MAILFORM_007", "Text line");
define("CMS_L_IFC_MAILFORM_008", "Text area");
define("CMS_L_IFC_MAILFORM_009", "New element");
define("CMS_L_IFC_MAILFORM_010", "Create form");
define("CMS_L_IFC_MAILFORM_011", "All");
define("CMS_L_IFC_MAILFORM_012", "Settings");
define("CMS_L_IFC_MAILFORM_013", "Recipient email");
define("CMS_L_IFC_MAILFORM_014", "Options");
define("CMS_L_IFC_MAILFORM_015", "Put + at the beginning of a line to preselect an option.");
define("CMS_L_IFC_MAILFORM_016", "Width");
define("CMS_L_IFC_MAILFORM_017", "Characters");
define("CMS_L_IFC_MAILFORM_018", "Default");
define("CMS_L_IFC_MAILFORM_019", "Input validation");
define("CMS_L_IFC_MAILFORM_020", "Name");
define("CMS_L_IFC_MAILFORM_021", "Email");
define("CMS_L_IFC_MAILFORM_022", "Phone/fax");
define("CMS_L_IFC_MAILFORM_023", "Input required");
define("CMS_L_IFC_MAILFORM_024", "Number of rows");
define("CMS_L_IFC_MAILFORM_025", "Description");
define("CMS_L_IFC_MAILFORM_026", "Confirmation text");
define("CMS_L_IFC_MAILFORM_027", "Target URL (HTTP POST)");
define("CMS_L_IFC_MAILFORM_028", "If no field ID is set, values are transmitted via HTTP POST " .
                                 "using <strong>mailform_value(n)</strong> in field order:" .
                                 "<strong>mailform_value1</strong>, <strong>mailform_value2</strong>, etc.<br>" .
                                 "<br>" .
                                 "The ID of the subject line is <strong>mailform_subject</strong>.");
define("CMS_L_IFC_MAILFORM_029", "Hidden field");
define("CMS_L_IFC_MAILFORM_030", "Field ID");
define("CMS_L_IFC_MAILFORM_031", "Number");
define("CMS_L_IFC_MAILFORM_032", "Contains email, send confirmation");
define("CMS_L_IFC_MAILFORM_033", "Page break");
define("CMS_L_IFC_MAILFORM_034", "Columns");
define("CMS_L_IFC_MAILFORM_035", "Custom validation (PCRE)");
define("CMS_L_IFC_MAILFORM_036", "Invert");
define("CMS_L_IFC_MAILFORM_037", "None");
define("CMS_L_IFC_MAILFORM_038", "Step 1: Settings");
define("CMS_L_IFC_MAILFORM_039", "Step 2: Position");
define("CMS_L_IFC_MAILFORM_040", "Set up connection");
define("CMS_L_IFC_MAILFORM_041", "SMTP server");
define("CMS_L_IFC_MAILFORM_042", "Email (From)");
define("CMS_L_IFC_MAILFORM_043", "User");
define("CMS_L_IFC_MAILFORM_044", "Password");
define("CMS_L_IFC_MAILFORM_045", "Send test mail");
define("CMS_L_IFC_MAILFORM_046", "Use CAPTCHA");
define("CMS_L_IFC_MAILFORM_047", "HTML code");
define("CMS_L_IFC_MAILFORM_048", "HTML");
define("CMS_L_IFC_MAILFORM_049", "The HTML code must contain a form field " .
                                 "whose NAME attribute matches the field ID." .
                                 "<br>" .
                                 "Available placeholders: " .
                                 "<strong>%label%</strong> field label, " .
                                 "<strong>%id%</strong> field ID, " .
                                 "<strong>%name%</strong> field name, " .
                                 "<strong>%value%</strong> field value");
define("CMS_L_IFC_MAILFORM_050", "Submit button labeling");
define("CMS_L_IFC_MAILFORM_051", "Show code only");
define("CMS_L_IFC_MAILFORM_052", "Use mail() function");
define("CMS_L_IFC_MAILFORM_053", "Use SMTP server");
define("CMS_L_IFC_MAILFORM_054", "SMTP settings");
define("CMS_L_IFC_MAILFORM_055", "[TEST] Automatically generated test message");
define("CMS_L_IFC_MAILFORM_056", "Do not show in confirmation");
define("CMS_L_IFC_MAILFORM_057", "URL");
define("CMS_L_IFC_MAILFORM_058", "Street/No.");
define("CMS_L_IFC_MAILFORM_059", "Drag here to delete");
define("CMS_L_IFC_MAILFORM_060", "This is an automatically generated test message, " .
                                 "to check the email sending function. " .
                                 "No action is required on your part. " .
                                 "You can safely delete this message. " .
                                 "Receiving this email confirms " .
                                 "that the sending was carried out correctly.");
define("CMS_L_IFC_MAILFORM_061", "Email (Reply-To)");

//ifc_media
//..................................................................................................
define("CMS_L_IFC_MEDIA",     "Media");
define("CMS_L_IFC_MEDIA_001", "Upload media");
define("CMS_L_IFC_MEDIA_002", "Media file");
define("CMS_L_IFC_MEDIA_003", "Add URL");
define("CMS_L_IFC_MEDIA_004", "Preview not available. " .
                              "IFRAMEs are disabled or not supported by your web software.");
define("CMS_L_IFC_MEDIA_005", "Rename category");
define("CMS_L_IFC_MEDIA_007", "Media");
define("CMS_L_IFC_MEDIA_008", "Replace media");
define("CMS_L_IFC_MEDIA_010", "Media formats");
define("CMS_L_IFC_MEDIA_011", "New media format");
define("CMS_L_IFC_MEDIA_012", "Media format");
define("CMS_L_IFC_MEDIA_013", "Assigned file extensions (comma-separated)");
define("CMS_L_IFC_MEDIA_014", "Embed code");
define("CMS_L_IFC_MEDIA_015", "Insert value …");
define("CMS_L_IFC_MEDIA_016", "The source is external and cannot be replaced internally. " .
                              "Select <em>Edit</em> to change the source.");
define("CMS_L_IFC_MEDIA_017", "Additional");
define("CMS_L_IFC_MEDIA_018", "File name");
define("CMS_L_IFC_MEDIA_019", "Category");
define("CMS_L_IFC_MEDIA_020", "Bulk upload");
define("CMS_L_IFC_MEDIA_021", "Here you can select multiple files for upload.");
define("CMS_L_IFC_MEDIA_025", "Data");

//ifc_permission
//..................................................................................................
define("CMS_L_IFC_PERMISSION",     "Administration");
define("CMS_L_IFC_PERMISSION_001", "Create account");
define("CMS_L_IFC_PERMISSION_002", "Username");
define("CMS_L_IFC_PERMISSION_003", "Password confirmation");
define("CMS_L_IFC_PERMISSION_004", "Create group");
define("CMS_L_IFC_PERMISSION_005", "Group ID");
define("CMS_L_IFC_PERMISSION_006", "Note");
define("CMS_L_IFC_PERMISSION_007", "Group membership");
define("CMS_L_IFC_PERMISSION_008", "Disabled");
define("CMS_L_IFC_PERMISSION_009", "Granted access");
define("CMS_L_IFC_PERMISSION_010", "Account");
define("CMS_L_IFC_PERMISSION_011", "Expiry (Days)");
define("CMS_L_IFC_PERMISSION_012", "Members");
define("CMS_L_IFC_PERMISSION_013", "Denied access takes priority.");
define("CMS_L_IFC_PERMISSION_014", "Permissions management");
define("CMS_L_IFC_PERMISSION_015", "Access granted");
define("CMS_L_IFC_PERMISSION_016", "Access denied");
define("CMS_L_IFC_PERMISSION_017", "Time zone");
define("CMS_L_IFC_PERMISSION_018", "Denied access");
define("CMS_L_IFC_PERMISSION_019", "Permissions");
define("CMS_L_IFC_PERMISSION_020", "Additional settings");
define("CMS_L_IFC_PERMISSION_021", "Users");
define("CMS_L_IFC_PERMISSION_022", "Groups");
define("CMS_L_IFC_PERMISSION_026", "Enable marked");
define("CMS_L_IFC_PERMISSION_027", "Disable marked");
define("CMS_L_IFC_PERMISSION_028", "Remove explicit permission");
define("CMS_L_IFC_PERMISSION_029", "Remove explizit exclusion");
define("CMS_L_IFC_PERMISSION_030", "<strong>⁺</strong> explicitly granted");
define("CMS_L_IFC_PERMISSION_031", "<strong>⁺</strong> explicitly granted, <strong>⁻</strong> explicitly denied");
define("CMS_L_IFC_PERMISSION_032", "Access key");
define("CMS_L_IFC_PERMISSION_033", "Password and confirmation do not match. " .
                                   "The password has not been changed.");

//ifc_profile
//..................................................................................................
define("CMS_L_IFC_PROFILE",     "Profiles");
define("CMS_L_IFC_PROFILE_001", "ID");
define("CMS_L_IFC_PROFILE_002", "Identifier");
define("CMS_L_IFC_PROFILE_003", "Username");
define("CMS_L_IFC_PROFILE_004", "Account inheritance");
define("CMS_L_IFC_PROFILE_005", "Organization");
define("CMS_L_IFC_PROFILE_006", "Forename");
define("CMS_L_IFC_PROFILE_007", "Surname");
define("CMS_L_IFC_PROFILE_008", "Street");
define("CMS_L_IFC_PROFILE_009", "Zip");
define("CMS_L_IFC_PROFILE_010", "City");
define("CMS_L_IFC_PROFILE_011", "Country");
define("CMS_L_IFC_PROFILE_012", "Phone");
define("CMS_L_IFC_PROFILE_013", "Phone 2");
define("CMS_L_IFC_PROFILE_014", "Cell phone");
define("CMS_L_IFC_PROFILE_015", "Fax");
define("CMS_L_IFC_PROFILE_016", "Remark");
define("CMS_L_IFC_PROFILE_017", "Starts with");
define("CMS_L_IFC_PROFILE_018", "Contains");
define("CMS_L_IFC_PROFILE_019", "Ends with");
define("CMS_L_IFC_PROFILE_020", "Greater than");
define("CMS_L_IFC_PROFILE_021", "Equal to");
define("CMS_L_IFC_PROFILE_022", "Less than");
define("CMS_L_IFC_PROFILE_023", "Grouping");
define("CMS_L_IFC_PROFILE_024", "Rows");
define("CMS_L_IFC_PROFILE_025", "Reset");
define("CMS_L_IFC_PROFILE_026", "(Empty)");
define("CMS_L_IFC_PROFILE_027", "User credentials");
define("CMS_L_IFC_PROFILE_028", "Personal data");
define("CMS_L_IFC_PROFILE_029", "Contact information");
define("CMS_L_IFC_PROFILE_030", "Miscellaneous");
define("CMS_L_IFC_PROFILE_031", "(No value)");
define("CMS_L_IFC_PROFILE_032", "Visible");
define("CMS_L_IFC_PROFILE_033", "Editable");
define("CMS_L_IFC_PROFILE_034", "Basic settings");
define("CMS_L_IFC_PROFILE_035", "Required");
define("CMS_L_IFC_PROFILE_036", "Additional settings");
define("CMS_L_IFC_PROFILE_037", "Additional information");
define("CMS_L_IFC_PROFILE_038", "Custom field");
define("CMS_L_IFC_PROFILE_039", "Filter");
define("CMS_L_IFC_PROFILE_040", "Page");
define("CMS_L_IFC_PROFILE_041", "Active");
define("CMS_L_IFC_PROFILE_042", "Credit card data");
define("CMS_L_IFC_PROFILE_043", "Banking details");
define("CMS_L_IFC_PROFILE_044", "Credit card number");
define("CMS_L_IFC_PROFILE_045", "Credit institution");
define("CMS_L_IFC_PROFILE_046", "Credit card holder");
define("CMS_L_IFC_PROFILE_047", "Expiry");
define("CMS_L_IFC_PROFILE_048", "Account number");
define("CMS_L_IFC_PROFILE_049", "Bank");
define("CMS_L_IFC_PROFILE_050", "Bank code");
define("CMS_L_IFC_PROFILE_051", "Account holder");
define("CMS_L_IFC_PROFILE_053", "Created");
define("CMS_L_IFC_PROFILE_054", "Updated");
define("CMS_L_IFC_PROFILE_055", "Generate");

//ifc_search
//..................................................................................................
define("CMS_L_IFC_SEARCH",     "Search");
define("CMS_L_IFC_SEARCH_001", "Filter");
define("CMS_L_IFC_SEARCH_002", "Title");
define("CMS_L_IFC_SEARCH_003", "Queue: <strong>%u tasks total</strong> " .
                               "<a href=\"%s\" class=\"green\">[Process]</a> " .
                               "<a href=\"%s\" class=\"red\">[Delete]</a>, " .
                               "<strong>%u internal pages</strong> " .
                               "<a href=\"%s\" class=\"green\">[Process]</a> " .
                               "<a href=\"%s\" class=\"red\">[Delete]</a>, " .
                               "<strong>%u backend tasks</strong> " .
                               "<a href=\"%s\" class=\"green\">[Process]</a> " .
                               "<a href=\"%s\" class=\"red\">[Delete]</a>, " .
                               "<strong>%u frontend tasks</strong> " .
                               "<a href=\"%s\" class=\"green\">[Process]</a> " .
                               "<a href=\"%s\" class=\"red\">[Delete]</a>, " .
                               "<strong>%u links</strong> " .
                               "<a href=\"%s\" class=\"green\">[Process]</a> " .
                               "<a href=\"%s\" class=\"red\">[Delete]</a>, " .
                               "<strong>%u updates</strong> " .
                               "<a href=\"%s\" class=\"green\">[Process]</a> " .
                               "<a href=\"%s\" class=\"red\">[Delete]</a>.");
define("CMS_L_IFC_SEARCH_004", "Content");
define("CMS_L_IFC_SEARCH_005", "As of");
define("CMS_L_IFC_SEARCH_006", "Starts with");
define("CMS_L_IFC_SEARCH_007", "Contains");
define("CMS_L_IFC_SEARCH_008", "Ends with");
define("CMS_L_IFC_SEARCH_009", "Greater than");
define("CMS_L_IFC_SEARCH_010", "Equal to");
define("CMS_L_IFC_SEARCH_011", "Less than");
define("CMS_L_IFC_SEARCH_012", "Rows");
define("CMS_L_IFC_SEARCH_013", "Redirect limit reached");
define("CMS_L_IFC_SEARCH_014", "Fetch");
define("CMS_L_IFC_SEARCH_015", "Update all");
define("CMS_L_IFC_SEARCH_016", "Capture links");
define("CMS_L_IFC_SEARCH_017", "Calculate scores");
define("CMS_L_IFC_SEARCH_018", "Add page");
define("CMS_L_IFC_SEARCH_019", "Process all");
define("CMS_L_IFC_SEARCH_020", "Process internal pages");
define("CMS_L_IFC_SEARCH_021", "Process frontend tasks");
define("CMS_L_IFC_SEARCH_022", "Process links");
define("CMS_L_IFC_SEARCH_023", "Process updates");
define("CMS_L_IFC_SEARCH_024", "Update");
define("CMS_L_IFC_SEARCH_025", "Optimize database");
define("CMS_L_IFC_SEARCH_026", "Register internal pages");
define("CMS_L_IFC_SEARCH_027", "Process backend tasks");
define("CMS_L_IFC_SEARCH_028", "Update marked");
define("CMS_L_IFC_SEARCH_029", "Loading …");
define("CMS_L_IFC_SEARCH_030", "Error");
define("CMS_L_IFC_SEARCH_031", "OK");
define("CMS_L_IFC_SEARCH_032", "Done");
define("CMS_L_IFC_SEARCH_033", "Invalid URL");
define("CMS_L_IFC_SEARCH_034", "No connection");
define("CMS_L_IFC_SEARCH_035", "Format not supported");
define("CMS_L_IFC_SEARCH_036", "No change");
define("CMS_L_IFC_SEARCH_037", "%s remaining");
define("CMS_L_IFC_SEARCH_038", "Data fetch failed");
define("CMS_L_IFC_SEARCH_039", "No content");
define("CMS_L_IFC_SEARCH_040", "Indexing prohibited");
define("CMS_L_IFC_SEARCH_041", "Indexed");
define("CMS_L_IFC_SEARCH_042", "Indexing failed");
define("CMS_L_IFC_SEARCH_043", "Unknown error");
define("CMS_L_IFC_SEARCH_044", "Reset");
define("CMS_L_IFC_SEARCH_045", "Sorting");
define("CMS_L_IFC_SEARCH_046", "Squeezing link juice …");
define("CMS_L_IFC_SEARCH_047", "Initializing scores");
define("CMS_L_IFC_SEARCH_048", "Calculating scores (%u/%u)");
define("CMS_L_IFC_SEARCH_049", "Finalizing scores");
define("CMS_L_IFC_SEARCH_050", "Integrating dangling links (%u/%u)");
define("CMS_L_IFC_SEARCH_051", "URL rejected");
define("CMS_L_IFC_SEARCH_052", "URL filters");
define("CMS_L_IFC_SEARCH_053", "Exclusion rules");
define("CMS_L_IFC_SEARCH_054", "Exception rules");
define("CMS_L_IFC_SEARCH_055", "Use regular expressions (" .
                               "<a href=\"https://www.pcre.org/pcre.txt\" " .
                                  "onclick=\"window.open(this.href);return false;\" " .
                                  "rel=\"noopener noreferrer\">" .
                               "PCRE</a>) " .
                               "to block URLs by specifying one expression per line. " .
                               "The exception rules define exceptions to these. " .
                               "Set a \"!\" at the beginning of a line to invert an expression.<br>" .
                               "<br>" .
                               "Example: !~^https://(?:[^\.]+\.)?example\.com(?:/\|$)~i<br>" .
                               "All URLs that do not belong to the domain example.com.");
define("CMS_L_IFC_SEARCH_056", "Find canonical pages");
define("CMS_L_IFC_SEARCH_057", "Execute");
define("CMS_L_IFC_SEARCH_058", "Score");
define("CMS_L_IFC_SEARCH_059", "Stop words");
define("CMS_L_IFC_SEARCH_060", "Stop words (whitespace-separated)");
define("CMS_L_IFC_SEARCH_061", "Define words without contentual relevance in the respective language, " .
                               "e.g. articles, conjunctions, prepositions, etc. " .
                               "Filtering these words allows for a more efficient search. " .
                               "Separate each word by whitespace (space, line break, etc.)");
define("CMS_L_IFC_SEARCH_062", "Settings");
define("CMS_L_IFC_SEARCH_063", "Max. deviation for similarity");
define("CMS_L_IFC_SEARCH_064", "(Default)");
define("CMS_L_IFC_SEARCH_065", "Daemon");
define("CMS_L_IFC_SEARCH_066", "Activate");
define("CMS_L_IFC_SEARCH_067", "Search");

//ifc_setup
//..................................................................................................
define("CMS_L_IFC_SETUP",     "System");
define("CMS_L_IFC_SETUP_001", "Administrator password");
define("CMS_L_IFC_SETUP_002", "The administrator password is still set to default and is not safe. " .
                              "Please specify a new password to log in as administrator.");
define("CMS_L_IFC_SETUP_003", "New password");
define("CMS_L_IFC_SETUP_004", "Password confirmation");
define("CMS_L_IFC_SETUP_005", "Database connection");
define("CMS_L_IFC_SETUP_006", "Please provide the credentials for the MySQL database " .
                              "that you want to use to store your application data.");
define("CMS_L_IFC_SETUP_007", "MySQL server");
define("CMS_L_IFC_SETUP_008", "MySQL database");
define("CMS_L_IFC_SETUP_009", "User");
define("CMS_L_IFC_SETUP_010", "Password");
define("CMS_L_IFC_SETUP_011", "Download Backup");
define("CMS_L_IFC_SETUP_012", "Email delivery");
define("CMS_L_IFC_SETUP_013", "Please provide the email address " .
                              "that will be used to send form data " .
                              "and choose the dispatch method.");
define("CMS_L_IFC_SETUP_014", "SMTP server");
define("CMS_L_IFC_SETUP_015", "Email (From)");
define("CMS_L_IFC_SETUP_016", "User");
define("CMS_L_IFC_SETUP_017", "Password");
define("CMS_L_IFC_SETUP_018", "No, skip step.");
define("CMS_L_IFC_SETUP_019", "Yes, write data.");
define("CMS_L_IFC_SETUP_020", "You have not specified a password.");
define("CMS_L_IFC_SETUP_021", "Password and confirmation do not match.");
define("CMS_L_IFC_SETUP_022", "Database connection could not be established.");
define("CMS_L_IFC_SETUP_023", "Email delivery server connection could not be established.");
define("CMS_L_IFC_SETUP_024", "Do not show setup after login anymore.");
define("CMS_L_IFC_SETUP_025", "Use mail() function");
define("CMS_L_IFC_SETUP_026", "Use SMTP server");
define("CMS_L_IFC_SETUP_027", "SMTP settings");
define("CMS_L_IFC_SETUP_028", "[TEST] Automatically generated test message");
define("CMS_L_IFC_SETUP_029", "Database initialization");
define("CMS_L_IFC_SETUP_030", "Do you want to write the initial data to the database (recommended)? " .
                              "Existing tables will not be overwritten, but will be backed up instead.");
define("CMS_L_IFC_SETUP_031", "The initial data could not be written to the database.");
define("CMS_L_IFC_SETUP_032", "Waiting for execution …");
define("CMS_L_IFC_SETUP_033", "The update version could not be determined.");
define("CMS_L_IFC_SETUP_034", "Try again");
define("CMS_L_IFC_SETUP_035", "The system is up to date.");
define("CMS_L_IFC_SETUP_036", "A newer version is available: <strong>%s</strong><br>" .
                              "Current version: %s");
define("CMS_L_IFC_SETUP_037", "Install update now");
define("CMS_L_IFC_SETUP_038", "Update installation");
define("CMS_L_IFC_SETUP_039", "No response from the server.");
define("CMS_L_IFC_SETUP_040", "Skip backup (not recommended)");
define("CMS_L_IFC_SETUP_041", "This is an automatically generated test message, " .
                              "to check the email sending function. " .
                              "No action is required on your part. " .
                              "You can safely delete this message. " .
                              "Receiving this email confirms " .
                              "that the sending was carried out correctly.");
define("CMS_L_IFC_SETUP_042", "Backup");
define("CMS_L_IFC_SETUP_043", "Create new backup");
define("CMS_L_IFC_SETUP_044", "Backup operation");
define("CMS_L_IFC_SETUP_045", "Daemon monitor");
define("CMS_L_IFC_SETUP_046", "Unknown error");
define("CMS_L_IFC_SETUP_047", "Email (Reply-To)");

//ifc_system
//..................................................................................................
define("CMS_L_IFC_SYSTEM",     "#System");
define("CMS_L_IFC_SYSTEM_001", "Attribute");
define("CMS_L_IFC_SYSTEM_002", "Allow ancillary elements");
define("CMS_L_IFC_SYSTEM_006", "Step 1: Settings");
define("CMS_L_IFC_SYSTEM_007", "Value");
define("CMS_L_IFC_SYSTEM_008", "Step 2: Position");
define("CMS_L_IFC_SYSTEM_009", "Index");

//ifc_template
//..................................................................................................
define("CMS_L_IFC_TEMPLATE",     "Templates");
define("CMS_L_IFC_TEMPLATE_001", "Source code from URL");
define("CMS_L_IFC_TEMPLATE_002", "Blank source code");
define("CMS_L_IFC_TEMPLATE_003", "Source code from file");
define("CMS_L_IFC_TEMPLATE_004", "Add stylesheet from template");
define("CMS_L_IFC_TEMPLATE_005", "Category");
define("CMS_L_IFC_TEMPLATE_006", "Source code");
define("CMS_L_IFC_TEMPLATE_007", "Add template");
define("CMS_L_IFC_TEMPLATE_008", "Page template");
define("CMS_L_IFC_TEMPLATE_009", "Preview not available. " .
                                 "IFRAMEs are disabled or not supported by your web software.");
define("CMS_L_IFC_TEMPLATE_010", "Add image");
define("CMS_L_IFC_TEMPLATE_011", "Add …");
define("CMS_L_IFC_TEMPLATE_012", "Keep source code");
define("CMS_L_IFC_TEMPLATE_013", "Add placeholder");
define("CMS_L_IFC_TEMPLATE_014", "Header data");
define("CMS_L_IFC_TEMPLATE_015", "Control pad");
define("CMS_L_IFC_TEMPLATE_016", "Conditional block");
define("CMS_L_IFC_TEMPLATE_017", "Conditional alternative");
define("CMS_L_IFC_TEMPLATE_018", "Repeat");
define("CMS_L_IFC_TEMPLATE_019", "Template");
define("CMS_L_IFC_TEMPLATE_020", "Text");
define("CMS_L_IFC_TEMPLATE_021", "Link");
define("CMS_L_IFC_TEMPLATE_022", "Image");
define("CMS_L_IFC_TEMPLATE_023", "Thumbnail");
define("CMS_L_IFC_TEMPLATE_024", "Media");
define("CMS_L_IFC_TEMPLATE_025", "Download");
define("CMS_L_IFC_TEMPLATE_026", "Plugin");
define("CMS_L_IFC_TEMPLATE_027", "Script");
define("CMS_L_IFC_TEMPLATE_028", "Editing only");
define("CMS_L_IFC_TEMPLATE_029", "Comment");
define("CMS_L_IFC_TEMPLATE_030", "Page frame");
define("CMS_L_IFC_TEMPLATE_031", "Backlink");
define("CMS_L_IFC_TEMPLATE_032", "HTML");
define("CMS_L_IFC_TEMPLATE_033", "PHP include");
define("CMS_L_IFC_TEMPLATE_034", "Rename category");
define("CMS_L_IFC_TEMPLATE_035", "Position shift");
define("CMS_L_IFC_TEMPLATE_036", "No caching");
define("CMS_L_IFC_TEMPLATE_037", "Group");
define("CMS_L_IFC_TEMPLATE_038", "Namespace");
define("CMS_L_IFC_TEMPLATE_039", "Base namespace");
define("CMS_L_IFC_TEMPLATE_040", "Value");
define("CMS_L_IFC_TEMPLATE_041", "CSS");
define("CMS_L_IFC_TEMPLATE_042", "Add link");
define("CMS_L_IFC_TEMPLATE_043", "PHP output");
define("CMS_L_IFC_TEMPLATE_044", "Save and display");
define("CMS_L_IFC_TEMPLATE_045", "Navigation");
define("CMS_L_IFC_TEMPLATE_046", "Add download");
define("CMS_L_IFC_TEMPLATE_047", "Add link (PHP code)");
define("CMS_L_IFC_TEMPLATE_048", "Add image (PHP code)");
define("CMS_L_IFC_TEMPLATE_049", "Add media");
define("CMS_L_IFC_TEMPLATE_050", "PHP");
define("CMS_L_IFC_TEMPLATE_051", "CMS elements");
define("CMS_L_IFC_TEMPLATE_052", "Switch");
define("CMS_L_IFC_TEMPLATE_053", "Output only");
define("CMS_L_IFC_TEMPLATE_054", "Stylesheet");
define("CMS_L_IFC_TEMPLATE_055", "JavaScript");
define("CMS_L_IFC_TEMPLATE_056", "Add JavaScript from template");

//ifc_token
//..................................................................................................
define("CMS_L_IFC_TOKEN",     "Placeholders");
define("CMS_L_IFC_TOKEN_001", "Category");
define("CMS_L_IFC_TOKEN_002", "Placeholders");
define("CMS_L_IFC_TOKEN_003", "Placeholder");
define("CMS_L_IFC_TOKEN_004", "Preview not available. " .
                              "IFRAMEs are disabled or not supported by your web software.");
define("CMS_L_IFC_TOKEN_005", "Insert parameter …");
define("CMS_L_IFC_TOKEN_006", "Replacement code");
define("CMS_L_IFC_TOKEN_007", "Code");
define("CMS_L_IFC_TOKEN_008", "Parameters (comma-separated)");
define("CMS_L_IFC_TOKEN_009", "Rename category");

//==================================================================================================
//   DESKTOP
//==================================================================================================

//desktop_address
//..................................................................................................
define("CMS_L_DESKTOP_ADDRESS_001", "Contact");
define("CMS_L_DESKTOP_ADDRESS_002", "Memo");
define("CMS_L_DESKTOP_ADDRESS_003", "Organization");
define("CMS_L_DESKTOP_ADDRESS_004", "Phone");
define("CMS_L_DESKTOP_ADDRESS_005", "Fax");
define("CMS_L_DESKTOP_ADDRESS_006", "Contact");

//desktop_appointment
//..................................................................................................
define("CMS_L_DESKTOP_APPOINTMENT_001", "New appointment");
define("CMS_L_DESKTOP_APPOINTMENT_002", "Appointment");
define("CMS_L_DESKTOP_APPOINTMENT_003", "Date");
define("CMS_L_DESKTOP_APPOINTMENT_004", "Time");
define("CMS_L_DESKTOP_APPOINTMENT_005", "Subject");
define("CMS_L_DESKTOP_APPOINTMENT_006", "Location");
define("CMS_L_DESKTOP_APPOINTMENT_007", "Date / Time");
define("CMS_L_DESKTOP_APPOINTMENT_011", "Remove after expiry");
define("CMS_L_DESKTOP_APPOINTMENT_012", "Contents");
define("CMS_L_DESKTOP_APPOINTMENT_013", "Participants");

//desktop_ims
//..................................................................................................
define("CMS_L_DESKTOP_IMS_001", "Compose message");
define("CMS_L_DESKTOP_IMS_002", "Recipient");
define("CMS_L_DESKTOP_IMS_003", "Message");
define("CMS_L_DESKTOP_IMS_004", "No recipients available.");
define("CMS_L_DESKTOP_IMS_005", "No messages available.");
define("CMS_L_DESKTOP_IMS_009", "Show user correspondence");
define("CMS_L_DESKTOP_IMS_010", "Show successional messages");
define("CMS_L_DESKTOP_IMS_011", "Reply");
define("CMS_L_DESKTOP_IMS_012", "Messenger");
define("CMS_L_DESKTOP_IMS_013", "to");
define("CMS_L_DESKTOP_IMS_014", "from");

//desktop_link
//..................................................................................................
define("CMS_L_DESKTOP_LINK_001", "Remove menu");
define("CMS_L_DESKTOP_LINK_002", "Link");
define("CMS_L_DESKTOP_LINK_003", "Reload");

//desktop_mailbox
//..................................................................................................
define("CMS_L_DESKTOP_MAILBOX_001", "In-box");
define("CMS_L_DESKTOP_MAILBOX_002", "Out-box");
define("CMS_L_DESKTOP_MAILBOX_003", "Drafts");
define("CMS_L_DESKTOP_MAILBOX_004", "Trash");
define("CMS_L_DESKTOP_MAILBOX_005", "Spam");
define("CMS_L_DESKTOP_MAILBOX_006", "%u new message(s), thereof %u identified as spam.");
define("CMS_L_DESKTOP_MAILBOX_007", "Message");
define("CMS_L_DESKTOP_MAILBOX_008", "Sender");
define("CMS_L_DESKTOP_MAILBOX_009", "RE");
define("CMS_L_DESKTOP_MAILBOX_010", "Send date");
define("CMS_L_DESKTOP_MAILBOX_011", "Recipient");
define("CMS_L_DESKTOP_MAILBOX_012", "Secondary recipient");
define("CMS_L_DESKTOP_MAILBOX_013", "<small>Format:</small> " .
                                    "<strong>%s</strong>, " .
                                    "<small>Size:</small> %s");
define("CMS_L_DESKTOP_MAILBOX_014", "Compose message");
define("CMS_L_DESKTOP_MAILBOX_015", "CC (Copy)");
define("CMS_L_DESKTOP_MAILBOX_016", "BCC (Blind copy)");
define("CMS_L_DESKTOP_MAILBOX_017", "Subject");
define("CMS_L_DESKTOP_MAILBOX_018", "Account name");
define("CMS_L_DESKTOP_MAILBOX_019", "Incoming mail server (POP3)");
define("CMS_L_DESKTOP_MAILBOX_020", "Outgoing mail server (SMTP)");
define("CMS_L_DESKTOP_MAILBOX_021", "Signature");
define("CMS_L_DESKTOP_MAILBOX_022", "Spam filter threshold");
define("CMS_L_DESKTOP_MAILBOX_023", "Disabled");
define("CMS_L_DESKTOP_MAILBOX_024", "Receive");
define("CMS_L_DESKTOP_MAILBOX_025", "Send");
define("CMS_L_DESKTOP_MAILBOX_026", "Create folder");
define("CMS_L_DESKTOP_MAILBOX_027", "Rename folder");
define("CMS_L_DESKTOP_MAILBOX_028", "Delete folder");
define("CMS_L_DESKTOP_MAILBOX_029", "Settings");
define("CMS_L_DESKTOP_MAILBOX_030", "Read message");
define("CMS_L_DESKTOP_MAILBOX_031", "Email");
define("CMS_L_DESKTOP_MAILBOX_032", "Folder");
define("CMS_L_DESKTOP_MAILBOX_036", "Move to");
define("CMS_L_DESKTOP_MAILBOX_037", "Date");
define("CMS_L_DESKTOP_MAILBOX_038", "Sort by date");
define("CMS_L_DESKTOP_MAILBOX_039", "Sender");
define("CMS_L_DESKTOP_MAILBOX_040", "Sort by Sender");
define("CMS_L_DESKTOP_MAILBOX_041", "Spam");
define("CMS_L_DESKTOP_MAILBOX_042", "Sort by spam probability");
define("CMS_L_DESKTOP_MAILBOX_043", "Size");
define("CMS_L_DESKTOP_MAILBOX_044", "Sort by size");
define("CMS_L_DESKTOP_MAILBOX_045", "Subject");
define("CMS_L_DESKTOP_MAILBOX_046", "Sort by subject");
define("CMS_L_DESKTOP_MAILBOX_048", "Train as spam");
define("CMS_L_DESKTOP_MAILBOX_049", "Train as normal");
define("CMS_L_DESKTOP_MAILBOX_050", "Empty trash");
define("CMS_L_DESKTOP_MAILBOX_051", "Delete messages from server after receipt");
define("CMS_L_DESKTOP_MAILBOX_053", "Attachment");
define("CMS_L_DESKTOP_MAILBOX_054", "-------- Original message --------");
define("CMS_L_DESKTOP_MAILBOX_055", "The to be forwarded message has been attached.");
define("CMS_L_DESKTOP_MAILBOX_056", "Reply");
define("CMS_L_DESKTOP_MAILBOX_057", "Forward");
define("CMS_L_DESKTOP_MAILBOX_058", "RE");
define("CMS_L_DESKTOP_MAILBOX_059", "FW");
define("CMS_L_DESKTOP_MAILBOX_060", "%u deleted message(s), thereof %u deleted from mail server.");
define("CMS_L_DESKTOP_MAILBOX_061", "Contains attachment");

//desktop_note
//..................................................................................................
define("CMS_L_DESKTOP_NOTE_001", "Memo");