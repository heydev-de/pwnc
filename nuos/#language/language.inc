<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   GERMAN LANGUAGE PACKAGE
//==================================================================================================

//language
//..................................................................................................
define("CMS_LANGUAGE_SEPARATOR",        chr(31)); //ascii unit separator

//error
//..................................................................................................
define("CMS_MSG_ERROR",                 "<strong>Fehler</strong><br>");
define("CMS_MSG_DONE",                  "<strong>OK</strong><br>");
define("CMS_MSG_UNAVAILABLE",           "Der Dienst ist nicht verfügbar.");

//common
//..................................................................................................
define("CMS_L_USER_ANONYMOUS",          "Gast");
define("CMS_L_USER_UNKNOWN",            "Unbekannt");
define("CMS_L_NOSCRIPT",                "Diese Anwendung ist nicht oder nur eingeschränkt verfügbar, " .
                                        "da JavaScript deaktiviert ist oder nicht von ihrer Websoftware unterstützt wird.");
define("CMS_L_NOCOOKIE",                "Diese Anwendung ist nicht oder nur eingeschränkt verfügbar, " .
                                        "da Cookies deaktiviert sind oder nicht von ihrer Websoftware unterstützt werden.");
define("CMS_L_NAME",                    "Name");
define("CMS_L_UNKNOWN",                 "Unbenannt");
define("CMS_L_PASSWORD",                "Passwort");
define("CMS_L_COLOR",                   "Farbe");
define("CMS_L_DESCRIPTION",             "Beschreibung");
define("CMS_L_EMAIL",                   "E-Mail");
define("CMS_L_IMAGE",                   "Bild");
define("CMS_L_MISSING",                 "Fehlend");
define("CMS_L_URL",                     "URL");
define("CMS_L_LANGUAGE_DEFAULT",        "default");
define("CMS_L_DATE_FORMAT",             "d.m.Y");
define("CMS_L_TIME_FORMAT",             "H:i:s");
define("CMS_L_DATETIME_FORMAT",         "d.m.Y, H:i:s");
define("CMS_L_SHORT_TIME_FORMAT",       "H:i");
define("CMS_L_SHORT_DATETIME_FORMAT",   "d.m.Y, H:i");
define("CMS_L_DECIMAL_SEPARATOR",       ",");
define("CMS_L_THOUSAND_SEPARATOR",      ".");

define("CMS_L_SUNDAY",                  "Sonntag");
define("CMS_L_MONDAY",                  "Montag");
define("CMS_L_TUESDAY",                 "Dienstag");
define("CMS_L_WEDNESDAY",               "Mittwoch");
define("CMS_L_THURSDAY",                "Donnerstag");
define("CMS_L_FRIDAY",                  "Freitag");
define("CMS_L_SATURDAY",                "Samstag");

define("CMS_L_SUNDAY_ABBR",             "So");
define("CMS_L_MONDAY_ABBR",             "Mo");
define("CMS_L_TUESDAY_ABBR",            "Di");
define("CMS_L_WEDNESDAY_ABBR",          "Mi");
define("CMS_L_THURSDAY_ABBR",           "Do");
define("CMS_L_FRIDAY_ABBR",             "Fr");
define("CMS_L_SATURDAY_ABBR",           "Sa");

define("CMS_L_JANUARY",                 "Januar");
define("CMS_L_FEBRUARY",                "Februar");
define("CMS_L_MARCH",                   "März");
define("CMS_L_APRIL",                   "April");
define("CMS_L_MAY",                     "Mai");
define("CMS_L_JUNE",                    "Juni");
define("CMS_L_JULY",                    "Juli");
define("CMS_L_AUGUST",                  "August");
define("CMS_L_SEPTEMBER",               "September");
define("CMS_L_OCTOBER",                 "Oktober");
define("CMS_L_NOVEMBER",                "November");
define("CMS_L_DECEMBER",                "Dezember");

define("CMS_L_JANUARY_ABBR",            "Jan");
define("CMS_L_FEBRUARY_ABBR",           "Feb");
define("CMS_L_MARCH_ABBR",              "Mär");
define("CMS_L_APRIL_ABBR",              "Apr");
define("CMS_L_MAY_ABBR",                "Mai");
define("CMS_L_JUNE_ABBR",               "Jun");
define("CMS_L_JULY_ABBR",               "Jul");
define("CMS_L_AUGUST_ABBR",             "Aug");
define("CMS_L_SEPTEMBER_ABBR",          "Sep");
define("CMS_L_OCTOBER_ABBR",            "Okt");
define("CMS_L_NOVEMBER_ABBR",           "Nov");
define("CMS_L_DECEMBER_ABBR",           "Dez");

define("CMS_L_ACCESS",                  "Zugriff");
define("CMS_L_READ",                    "Lesen");
define("CMS_L_WRITE",                   "Schreiben");
define("CMS_L_OPERATOR",                "Operator");

define("CMS_L_ALL",                     "Alle");
define("CMS_L_INVERT",                  "Umkehren");
define("CMS_L_NONE",                    "Keine");

//common commands
//..................................................................................................
define("CMS_L_COMMAND_ADD",             "Erstellen");
define("CMS_L_COMMAND_APPEND",          "Anhängen");
define("CMS_L_COMMAND_BOTTOM",          "Ende");
define("CMS_L_COMMAND_BUFFER",          "Zwischenspeichern");
define("CMS_L_COMMAND_CANCEL",          "Abbrechen");
define("CMS_L_COMMAND_CLOSE",           "Schließen");
define("CMS_L_COMMAND_CONFIRM",         "OK");
define("CMS_L_COMMAND_CONNECT",         "Verbinden");
define("CMS_L_COMMAND_COPY",            "Kopieren");
define("CMS_L_COMMAND_CUT",             "Verschieben");
define("CMS_L_COMMAND_DELETE",          "Löschen");
define("CMS_L_COMMAND_DELETE_SELECTED", "Markierte löschen");
define("CMS_L_COMMAND_EDIT",            "Bearbeiten");
define("CMS_L_COMMAND_END",             "Ende");
define("CMS_L_COMMAND_EXPORT",          "Exportieren");
define("CMS_L_COMMAND_INSERT",          "Einsetzen");
define("CMS_L_COMMAND_NEXT",            "Weiter");
define("CMS_L_COMMAND_PASTE",           "Einfügen");
define("CMS_L_COMMAND_PREVIOUS",        "Zurück");
define("CMS_L_COMMAND_PRINT",           "Drucken");
define("CMS_L_COMMAND_REFRESH",         "Aktualisieren");
define("CMS_L_COMMAND_SAVE",            "Speichern");
define("CMS_L_COMMAND_SELECT",          "Auswählen");
define("CMS_L_COMMAND_SHOW",            "Anzeigen");
define("CMS_L_COMMAND_START",           "Anfang");
define("CMS_L_COMMAND_TOP",             "Anfang");

//==================================================================================================
//   SYSTEM
//==================================================================================================

//common
//..................................................................................................
define("CMS_L_COMMON_001", "SYSTEMNACHRICHT");
define("CMS_L_COMMON_002", "vor %u Sek.");
define("CMS_L_COMMON_003", "vor %u Min.");
define("CMS_L_COMMON_004", "Gestern, %s");
define("CMS_L_COMMON_005", "%s, %s");
define("CMS_L_COMMON_006", "%s, vor %u Tagen, %s");
define("CMS_L_COMMON_007", "%u. %s, %s");
define("CMS_L_COMMON_008", "Ja");
define("CMS_L_COMMON_009", "Nein");
define("CMS_L_COMMON_010", "Freigabe");
define("CMS_L_COMMON_011", "Einblendungen");
define("CMS_L_COMMON_012", "in %u Sek.");
define("CMS_L_COMMON_013", "in %u Min.");
define("CMS_L_COMMON_014", "Morgen, %s");
define("CMS_L_COMMON_015", "vor %u Std., %u Min.");
define("CMS_L_COMMON_016", "%s, in %u Tagen, %s");
define("CMS_L_COMMON_017", "in %u Std., %u Min.");
define("CMS_L_COMMON_018", "vor %u Std.");
define("CMS_L_COMMON_019", "in %u Std.");
define("CMS_L_COMMON_020", "Gestern");
define("CMS_L_COMMON_021", "Morgen");
define("CMS_L_COMMON_022", "%s");
define("CMS_L_COMMON_023", "%s, in %u Tagen");
define("CMS_L_COMMON_024", "%s, vor %u Tagen");
define("CMS_L_COMMON_025", "%u. %s");
define("CMS_L_COMMON_026", "Heute");
define("CMS_L_COMMON_027", "Jetzt");
define("CMS_L_COMMON_028", "Vorschau");
define("CMS_L_COMMON_029", "Vorschau nicht verfügbar. " .
                           "IFRAMEs sind deaktiviert oder werden von ihrer Websoftware nicht unterstützt.");

//content_pool
//..................................................................................................
define("CMS_L_CONTENT_POOL_001", "Neuer Eintrag");

//core
//..................................................................................................
define("CMS_L_CORE_001", "%s ist eingestiegen.");
define("CMS_L_CORE_002", "%s ist ausgestiegen.");
define("CMS_L_CORE_003", "%s betritt die Gruppe %s.");
define("CMS_L_CORE_004", "%s verlässt die Gruppe %s.");
define("CMS_L_CORE_005", "Die Gruppe %s wurde erstellt.");
define("CMS_L_CORE_006", "Die Einstellungen der Gruppe %s wurden gespeichert.");
define("CMS_L_CORE_007", "Die Gruppe %s wurde geschlossen.");
define("CMS_L_CORE_008", "Das Profil wurde gespeichert.");
define("CMS_L_CORE_009", "%s in der Gruppe %s ist nun %s.");
define("CMS_L_CORE_010", "Gruppen-Admin");
define("CMS_L_CORE_011", "Moderator");
define("CMS_L_CORE_012", "abwesend");
define("CMS_L_CORE_013", "stumm");
define("CMS_L_CORE_014", "gesperrt");
define("CMS_L_CORE_015", "Mithörer");
define("CMS_L_CORE_016", "%s in der Gruppe %s ist nicht länger %s.");
define("CMS_L_CORE_017", "unsichtbar");

//core_control
//..................................................................................................
define("CMS_L_CORE_CONTROL_001", "Gruppen");
define("CMS_L_CORE_CONTROL_002", "Teilnehmer");
define("CMS_L_CORE_CONTROL_003", "Keine Einstellungen verfügbar.");
define("CMS_L_CORE_CONTROL_004", "Gruppen-Admin");
define("CMS_L_CORE_CONTROL_005", "Moderator");
define("CMS_L_CORE_CONTROL_006", "Mitglied");
define("CMS_L_CORE_CONTROL_007", "Gast");
define("CMS_L_CORE_CONTROL_008", "Teilnehmerprofil");
define("CMS_L_CORE_CONTROL_009", "Temporäre Gruppen");
define("CMS_L_CORE_CONTROL_010", "%u Teilnehmer");
define("CMS_L_CORE_CONTROL_011", "Passwort erforderlich");
define("CMS_L_CORE_CONTROL_012", "Beitreten");
define("CMS_L_CORE_CONTROL_013", "Ein Passwort ist erforderlich, um dieser Gruppe beizutreten.");
define("CMS_L_CORE_CONTROL_014", "Sie wurden von der Gruppe %s ausgeschlossen.");
define("CMS_L_CORE_CONTROL_015", "Gruppe bearbeiten");
define("CMS_L_CORE_CONTROL_016", "Gruppe schließen");
define("CMS_L_CORE_CONTROL_017", "Gruppe erstellen");
define("CMS_L_CORE_CONTROL_018", "Permanente Gruppe");
define("CMS_L_CORE_CONTROL_019", "Die gewählte Gruppe ist nicht verfügbar.");
define("CMS_L_CORE_CONTROL_020", "Klicken Sie auf den Button, um die Gruppe %s zu schließen.");
define("CMS_L_CORE_CONTROL_021", "Foto");
define("CMS_L_CORE_CONTROL_025", "Einstellungen");
define("CMS_L_CORE_CONTROL_026", "Anwenden");
define("CMS_L_CORE_CONTROL_027", "Erstellen");
define("CMS_L_CORE_CONTROL_028", "Mithörer");
define("CMS_L_CORE_CONTROL_029", "Unsichtbar");
define("CMS_L_CORE_CONTROL_030", "Abwesend");
define("CMS_L_CORE_CONTROL_031", "Stumm");
define("CMS_L_CORE_CONTROL_032", "Gesperrt");
define("CMS_L_CORE_CONTROL_035", "Privatnachricht");
define("CMS_L_CORE_CONTROL_036", "Empfänger");
define("CMS_L_CORE_CONTROL_037", "Nachricht");
define("CMS_L_CORE_CONTROL_038", "Senden");
define("CMS_L_CORE_CONTROL_040", "Administration");
define("CMS_L_CORE_CONTROL_041", "Profil");
define("CMS_L_CORE_CONTROL_042", "Profiltext");
define("CMS_L_CORE_CONTROL_043", "Foto hochladen");
define("CMS_L_CORE_CONTROL_044", "GIF, JPG, PNG, WebP, max. %s");
define("CMS_L_CORE_CONTROL_045", "Teilnehmer auswählen");
define("CMS_L_CORE_CONTROL_046", "Der gewählte Teilnehmer ist nicht verfügbar.");
define("CMS_L_CORE_CONTROL_047", "Admin");
define("CMS_L_CORE_CONTROL_048", "Geben Sie einen Namen für die zu erstellende Gruppe an. " .
                                 "Sollte die Gruppe bereits existieren, wird versucht, dieser beizutreten.");
define("CMS_L_CORE_CONTROL_049", "Die ausgewählte Datei ist zu groß.");
define("CMS_L_CORE_CONTROL_051", "Gruppe moderieren");
define("CMS_L_CORE_CONTROL_052", "Teilnehmer moderieren");

//desktop
//..................................................................................................
define("CMS_L_DESKTOP_001", "Posteingang");
define("CMS_L_DESKTOP_002", "Termine");
define("CMS_L_DESKTOP_003", "Kontakte");

//directory
//..................................................................................................
define("CMS_L_DIRECTORY_002", "Start");

//flexview
//..................................................................................................
define("CMS_L_FLEXVIEW_001", "(Leer)");
define("CMS_L_FLEXVIEW_002", "Root");
define("CMS_L_FLEXVIEW_003", "Hier einfügen");

//forum
//..................................................................................................
define("CMS_L_FORUM_001", "Antwort erhalten");
define("CMS_L_FORUM_002", "Sie erhielten eine Antwort zu ihrem Beitrag \"%s\" unter %s");

//ifc
//..................................................................................................
define("CMS_L_IFC_001", "Diese Anwendung ist nicht aktiv.");
define("CMS_L_IFC_002", "Beenden");
define("CMS_L_IFC_003", "Fenster wird geschlossen …");
define("CMS_L_IFC_004", "Erweitern/zurücksetzen");
define("CMS_L_IFC_005", "Ungültiger Wert");
define("CMS_L_IFC_006", "Desktop");
define("CMS_L_IFC_007", "Abmelden");
define("CMS_L_IFC_008", "Website");
define("CMS_L_IFC_009", "Freigabe");
define("CMS_L_IFC_010", "[Alt] + [W] : Whitespace bereinigen\n" .
                        "[Alt] + [F] : Auto-Formatierung");
define("CMS_L_IFC_011", "Update %s verfügbar.");
define("CMS_L_IFC_012", "Eingabe");
define("CMS_L_IFC_013", "Die Nutzung von PWNC ist und bleibt kostenfrei.<br>" .
                        "<a href=\"" . x(CMS_HOMEPAGE) . "/de/support.php\" " .
                           "onclick=\"window.open(this.href);return false;\">" .
                        "Bitte unterstützen Sie die Weiterentwicklung mit einem freiwilligen Beitrag." .
                        "</a>");

//log
//..................................................................................................
define("CMS_L_LOG_001", "Es wurden keine Datensätze gefunden.");
define("CMS_L_LOG_002", "Summe: %s (100,0%%)");
define("CMS_L_LOG_003", "%u Zeilen, %u Spalten; Ausführungszeit: %01.10f Sekunden");
define("CMS_L_LOG_004", "%u Zeilen bearbeitet; Ausführungszeit: %01.10f Sekunden");

//media_type
//..................................................................................................
define("CMS_L_MEDIA_TYPE_001", "(Automatisch)");
define("CMS_L_MEDIA_TYPE_002", "Die Resource <em>%s</em> wird geladen …");
define("CMS_L_MEDIA_TYPE_003", "Die Resource <a href=\"%s\"><em>%s</em></a> kann nicht dargestellt werden. " .
                               "Sie benötigen eventuell geeignete Zusatzsoftware.");

//permission
//..................................................................................................
define("CMS_L_PERMISSION_001", "Administrator");
define("CMS_L_PERMISSION_002", "Hauptverwaltungskonto");
define("CMS_L_PERMISSION_003", "Berechtigungen für nicht identifizierte Benutzer");
define("CMS_L_PERMISSION_004", "Benutzerregistrierung");
define("CMS_L_PERMISSION_005", "Default-Berechtigungen für die Benutzerregistrierung");
define("CMS_L_PERMISSION_006", "Daemon");
define("CMS_L_PERMISSION_007", "Ausführungskonto für Hintergrundaufgaben");

//pop
//..................................................................................................
define("CMS_L_POP_001", "Der Host %s ist nicht verfügbar.");

//rss_parser
//..................................................................................................
define("CMS_L_RSS_PARSER_001", "Aktualisiert");
define("CMS_L_RSS_PARSER_002", "Stand");
define("CMS_L_RSS_PARSER_003", "Generator");
define("CMS_L_RSS_PARSER_004", "Redakteur");
define("CMS_L_RSS_PARSER_005", "Webmaster");
define("CMS_L_RSS_PARSER_006", "Kommentare");
define("CMS_L_RSS_PARSER_007", "Von");
define("CMS_L_RSS_PARSER_008", "Quelle");
define("CMS_L_RSS_PARSER_009", "Permanenter Link");
define("CMS_L_RSS_PARSER_010", "ID");

//smtp
//..................................................................................................
define("CMS_L_SMTP_001", "Der Host %s ist nicht verfügbar.");
define("CMS_L_SMTP_002", "Der Host %s ist ungültig.");
define("CMS_L_SMTP_003", "Die mail()-Funktion ist fehlgeschlagen.");

//template
//..................................................................................................
define("CMS_L_TEMPLATE_001", "Neuer Eintrag");
define("CMS_L_TEMPLATE_002", "Vorschau");
define("CMS_L_TEMPLATE_003", "Plugin");
define("CMS_L_TEMPLATE_004", "Bedingte Bereiche");
define("CMS_L_TEMPLATE_005", "Links");
define("CMS_L_TEMPLATE_006", "Plugins");
define("CMS_L_TEMPLATE_007", "Texte");
define("CMS_L_TEMPLATE_008", "Downloads");
define("CMS_L_TEMPLATE_009", "Bilder");
define("CMS_L_TEMPLATE_010", "Thumbnails");
define("CMS_L_TEMPLATE_011", "Templates");
define("CMS_L_TEMPLATE_012", "Voll");
define("CMS_L_TEMPLATE_013", "Ausgabe");
define("CMS_L_TEMPLATE_015", "Layout");
define("CMS_L_TEMPLATE_016", "Inhalt");
define("CMS_L_TEMPLATE_017", "Bearbeitungsmodus");
define("CMS_L_TEMPLATE_018", "Vertauschen");
define("CMS_L_TEMPLATE_019", "Aktivieren");
define("CMS_L_TEMPLATE_020", "Wiederholungen");
define("CMS_L_TEMPLATE_021", "Media");
define("CMS_L_TEMPLATE_022", "Verschieben");
define("CMS_L_TEMPLATE_023", "Abhängige Alternativen");
define("CMS_L_TEMPLATE_024", "Einfügen und Vorige schieben");
define("CMS_L_TEMPLATE_025", "Einfügen und Folgende schieben");
define("CMS_L_TEMPLATE_026", "Entfernen und Folgende nachrücken");
define("CMS_L_TEMPLATE_027", "Entfernen und Vorige nachrücken");
define("CMS_L_TEMPLATE_028", "Gruppen");
define("CMS_L_TEMPLATE_029", "Text");
define("CMS_L_TEMPLATE_030", "Download");
define("CMS_L_TEMPLATE_031", "Bild");
define("CMS_L_TEMPLATE_032", "Thumbnail");
define("CMS_L_TEMPLATE_033", "Media");
define("CMS_L_TEMPLATE_034", "Template");
define("CMS_L_TEMPLATE_035", "Gruppe");
define("CMS_L_TEMPLATE_036", "Wiederholen");
define("CMS_L_TEMPLATE_037", "Verschieben");
define("CMS_L_TEMPLATE_038", "Schalter"); //singular
define("CMS_L_TEMPLATE_039", "Link");
define("CMS_L_TEMPLATE_040", "Sync-Quelle erstellen");
define("CMS_L_TEMPLATE_041", "Sync-Quelle verknüpfen");
define("CMS_L_TEMPLATE_042", "Synchronisiert");
define("CMS_L_TEMPLATE_043", "Wert");
define("CMS_L_TEMPLATE_044", "Werte");
define("CMS_L_TEMPLATE_045", "Doppelklick: Bearbeiten\n" .
                             "Ziehen &amp; Ablegen: Verschieben\n" .
                             "[Alt] + Ziehen &amp; Ablegen: Duplizieren");
define("CMS_L_TEMPLATE_046", "Klick: Verstecken / Zeigen\n" .
                             "[Alt] + Klick: Exklusiv anzeigen");
define("CMS_L_TEMPLATE_047", "Debug");
define("CMS_L_TEMPLATE_048", "Funktionen");
define("CMS_L_TEMPLATE_049", "Geändert");
define("CMS_L_TEMPLATE_050", "(Leer)");
define("CMS_L_TEMPLATE_051", "Aktive Elemente");
define("CMS_L_TEMPLATE_052", "Sprache");
define("CMS_L_TEMPLATE_053", "Aus");
define("CMS_L_TEMPLATE_054", "Ein");
define("CMS_L_TEMPLATE_055", "Schalter"); //plural
define("CMS_L_TEMPLATE_056", "%s herunterladen");

//update
//..................................................................................................
define("CMS_L_UPDATE_001", "Der Ordner %s konnte nicht erstellt werden.");
define("CMS_L_UPDATE_002", "Die Datei %s konnte nicht erstellt werden.");
define("CMS_L_UPDATE_003", "Der Ordner %s konnte nicht geöffnet werden.");
define("CMS_L_UPDATE_004", "Der Ordner %s konnte nicht zum Archiv %s hinzugefügt werden.");
define("CMS_L_UPDATE_005", "Die Datei %s konnte nicht zum Archiv %s hinzugefügt werden.");
define("CMS_L_UPDATE_006", "Die Datei %s konnte nicht ordnungsgemäß geschlossen werden.");
define("CMS_L_UPDATE_007", "Die Datei %s konnte nicht gelöscht werden.");
define("CMS_L_UPDATE_008", "Die Datei %s konnte nicht in %s umbenannt werden.");
define("CMS_L_UPDATE_009", "Sicherung wird erstellt …");
define("CMS_L_UPDATE_010", "Sicherung erfolgreich abgeschlossen.");
define("CMS_L_UPDATE_011", "Sicherung mit Fehlern abgeschlossen.");
define("CMS_L_UPDATE_012", "Update wird heruntergeladen …");
define("CMS_L_UPDATE_013", "Verbindung zum Update-Server konnte nicht hergestellt werden.");
define("CMS_L_UPDATE_014", "Exklusiver Zugriff auf die Datei %s war nicht möglich.");
define("CMS_L_UPDATE_015", "Das Update konnte nicht heruntergeladen werden.");
define("CMS_L_UPDATE_016", "Update erfolgreich heruntergeladen.");
define("CMS_L_UPDATE_017", "Die Datei %s konnte nicht geöffnet werden.");
define("CMS_L_UPDATE_018", "Die Datei %s konnte nicht aus dem Archiv %s extrahiert werden.");
define("CMS_L_UPDATE_019", "Update wird installiert …");
define("CMS_L_UPDATE_020", "Update konnte nicht installiert werden.");
define("CMS_L_UPDATE_021", "Update erfolgreich installiert.");
define("CMS_L_UPDATE_022", "Das Archiv %s ist leer.");
define("CMS_L_UPDATE_023", "Temporäre Daten werden entfernt …");
define("CMS_L_UPDATE_024", "Temporäre Daten wurden entfernt.");
define("CMS_L_UPDATE_025", "Update abgeschlossen.");
define("CMS_L_UPDATE_026", "Sicherung wurde übersprungen.");
define("CMS_L_UPDATE_027", "Datenbank wird exportiert …");
define("CMS_L_UPDATE_028", "Datenbank konnte nicht exportiert werden.");
define("CMS_L_UPDATE_029", "Datenbank erfolgreich exportiert.");
define("CMS_L_UPDATE_030", "Die Wiederherstellung der vorherigen Version ist fehlgeschlagen.");
define("CMS_L_UPDATE_031", "%s konnte nicht wiederhergestellt werden.");
define("CMS_L_UPDATE_032", "Die vorherige Version wird wiederhergestellt …");
define("CMS_L_UPDATE_033", "Das heruntergeladene Update ist ungültig.");

//==================================================================================================
//   MODULE
//==================================================================================================

//mod_blog
//..................................................................................................
define("CMS_L_MOD_BLOG_001", "Weitere Artikel");
define("CMS_L_MOD_BLOG_002", "Artikel schreiben");
define("CMS_L_MOD_BLOG_003", "Titel");
define("CMS_L_MOD_BLOG_004", "Tags");
define("CMS_L_MOD_BLOG_005", "Speichern und anzeigen");
define("CMS_L_MOD_BLOG_006", "Der Artikel wurde gelöscht.");
define("CMS_L_MOD_BLOG_007", "Der Artikel wurde gespeichert.");
define("CMS_L_MOD_BLOG_008", "Der Artikel wurde erstellt.");
define("CMS_L_MOD_BLOG_009", "Artikel bearbeiten");
define("CMS_L_MOD_BLOG_010", "Weiterlesen");
define("CMS_L_MOD_BLOG_011", "Übersicht");
define("CMS_L_MOD_BLOG_012", "Der Artikel konnte nicht gelöscht werden.");
define("CMS_L_MOD_BLOG_013", "Der Artikel konnte nicht gespeichert werden.");
define("CMS_L_MOD_BLOG_014", "Der Artikel konnte nicht erstellt werden.");
define("CMS_L_MOD_BLOG_015", "%u Artikel");
define("CMS_L_MOD_BLOG_016", "Code bearbeiten");
define("CMS_L_MOD_BLOG_017", "Der Code wurde gespeichert.");
define("CMS_L_MOD_BLOG_018", "Der Code konnte nicht gespeichert werden.");
define("CMS_L_MOD_BLOG_019", "Archiv");
define("CMS_L_MOD_BLOG_020", "Tags");
define("CMS_L_MOD_BLOG_021", "Alle anzeigen");
define("CMS_L_MOD_BLOG_022", "Veröffentlichung");
define("CMS_L_MOD_BLOG_023", "Sofort");
define("CMS_L_MOD_BLOG_024", "Deaktiviert");
define("CMS_L_MOD_BLOG_025", "Tag");
define("CMS_L_MOD_BLOG_026", "Monat");
define("CMS_L_MOD_BLOG_027", "Jahr");
define("CMS_L_MOD_BLOG_028", "Stunde");
define("CMS_L_MOD_BLOG_029", "Möchten Sie diesen Artikel wirklich löschen?");
define("CMS_L_MOD_BLOG_030", "RSS-Feed");
define("CMS_L_MOD_BLOG_031", "Blog");
define("CMS_L_MOD_BLOG_032", "Platzhalter");
define("CMS_L_MOD_BLOG_033", "von %s");
define("CMS_L_MOD_BLOG_034", "Autor");
define("CMS_L_MOD_BLOG_035", "Benutzername");
define("CMS_L_MOD_BLOG_036", "Der angeforderte Artikel ist nicht (mehr) verfügbar.");
define("CMS_L_MOD_BLOG_037", "Hervorheben");
define("CMS_L_MOD_BLOG_038", "%u Kommentare");
define("CMS_L_MOD_BLOG_039", "Minute");
define("CMS_L_MOD_BLOG_040", "Inhalt");
define("CMS_L_MOD_BLOG_041", "HTML-Code");
define("CMS_L_MOD_BLOG_042", "Geplant");
define("CMS_L_MOD_BLOG_043", "Artikel nicht gefunden");
define("CMS_L_MOD_BLOG_044", "Fehler");
define("CMS_L_MOD_BLOG_045", "Es ist ein Fehler aufgetreten.");

//mod_chat
//..................................................................................................
define("CMS_L_MOD_CHAT_001", "<p>" .
                             "Eine Verbindung ist zur Zeit nicht möglich, " .
                             "da ihre ID belegt ist." .
                             "</p>" .
                             "<p>" .
                             "<strong>" .
                             "Bitte haben Sie etwas Geduld …" .
                             "</strong>" .
                             "</p>" .
                             "<p>" .
                             "Sobald ihre ID wieder frei ist, " .
                             "gelangen Sie automatisch zur Anmeldung." .
                             "</p>");
define("CMS_L_MOD_CHAT_002", "Private Nachricht");
define("CMS_L_MOD_CHAT_003", "Der Name wird bereits verwendet.");
define("CMS_L_MOD_CHAT_004", "Sie wurden abgemeldet.");
define("CMS_L_MOD_CHAT_005", "%s schreibt etwas.");
define("CMS_L_MOD_CHAT_006", "Klänge abspielen");
define("CMS_L_MOD_CHAT_007", "Zurück");
define("CMS_L_MOD_CHAT_008", "Geben Sie den Namen an, den Sie verwenden möchten.");
define("CMS_L_MOD_CHAT_009", "Abwesend setzen");
define("CMS_L_MOD_CHAT_011", "Fokus bei Nachricht");
define("CMS_L_MOD_CHAT_012", "Steuerung");
define("CMS_L_MOD_CHAT_013", "Loggen Sie sich hier ein, " .
                             "wenn Sie ein Benutzerkonto haben.");
define("CMS_L_MOD_CHAT_014", "Chat");
define("CMS_L_MOD_CHAT_015", "Erneut einloggen");
define("CMS_L_MOD_CHAT_016", "Bei Inaktivität abwesend setzen");
define("CMS_L_MOD_CHAT_017", "Ausloggen");
define("CMS_L_MOD_CHAT_018", "Emojis");
define("CMS_L_MOD_CHAT_019", "<p>" .
                             "Eine Verbindung ist zur Zeit nicht möglich." .
                             "</p>" .
                             "<strong>" .
                             "Entweder liegt ein technisches Problem vor " .
                             "oder Sie wurden ausgeschlossen." .
                             "</strong>" .
                             "</p>");

//mod_comment
//..................................................................................................
define("CMS_L_MOD_COMMENT_001", "Bitte geben Sie ihren Namen an.");
define("CMS_L_MOD_COMMENT_002", "Bitte geben Sie eine gültige E-Mail ein.");
define("CMS_L_MOD_COMMENT_003", "Weitere Kommentare");
define("CMS_L_MOD_COMMENT_004", "Ihr Kommentar enthält keinen Text.");
define("CMS_L_MOD_COMMENT_005", "Bitte geben Sie einen Namen an.");
define("CMS_L_MOD_COMMENT_006", "Der Kommentar enthält keinen Text.");
define("CMS_L_MOD_COMMENT_007", "Kommentar bearbeiten");
define("CMS_L_MOD_COMMENT_008", "Wird nicht veröffentlicht.");
define("CMS_L_MOD_COMMENT_009", "Homepage-URL");
define("CMS_L_MOD_COMMENT_010", "Kommentar");
define("CMS_L_MOD_COMMENT_011", "Kommentar schreiben");
define("CMS_L_MOD_COMMENT_012", "%u Kommentar(e)");
define("CMS_L_MOD_COMMENT_013", "Soll der Kommentar gelöscht werden?");
define("CMS_L_MOD_COMMENT_015", "Deaktivieren");
define("CMS_L_MOD_COMMENT_016", "Aktivieren");
define("CMS_L_MOD_COMMENT_017", "ID: %s");
define("CMS_L_MOD_COMMENT_018", "Homepage");
define("CMS_L_MOD_COMMENT_020", "Spam");
define("CMS_L_MOD_COMMENT_021", "Soll der Kommentar als Spam bewertet und gelöscht werden?");
define("CMS_L_MOD_COMMENT_022", "Ihr Kommentar konnte nicht gespeichert werden.");
define("CMS_L_MOD_COMMENT_023", "Der Kommentar konnte nicht gespeichert werden.");
define("CMS_L_MOD_COMMENT_024", "Die Anweisung konnte nicht ausgeführt werden.");
define("CMS_L_MOD_COMMENT_025", "Positiv bewerten");
define("CMS_L_MOD_COMMENT_026", "Negativ bewerten");
define("CMS_L_MOD_COMMENT_027", "Neuer Kommentar (Freischaltung erforderlich)");
define("CMS_L_MOD_COMMENT_028", "Neuer Kommentar");
define("CMS_L_MOD_COMMENT_029", "Sie haben einen neuen Kommentar unter %s erhalten, " .
                                "der erst nach Freischaltung angezeigt wird:\n\n%s\n\n" .
                                "Klicken Sie hier, um den Kommentar freizuschalten:\n%s\n\n" .
                                "Klicken Sie hier, um den Kommentar abzulehnen:\n%s\n\n" .
                                "Klicken Sie hier, wenn es sich bei dem Kommentar um Spam handelt:\n%s");
define("CMS_L_MOD_COMMENT_030", "Sie haben einen neuen Kommentar unter %s erhalten:\n\n%s\n\n" .
                                "Klicken Sie hier, um den Kommentar auszublenden:\n%s\n\n" .
                                "Klicken Sie hier, um den Kommentar zu löschen:\n%s\n\n" .
                                "Klicken Sie hier, wenn es sich bei dem Kommentar um Spam handelt:\n%s");
define("CMS_L_MOD_COMMENT_031", "Ihr Kommentar wurde gespeichert.");
define("CMS_L_MOD_COMMENT_032", "Ihr Kommentar wurde gespeichert und ist erst sichtbar, " .
                                "nachdem er durch einen Moderator freigegeben wurde.");
define("CMS_L_MOD_COMMENT_033", "Die Änderungen wurden gespeichert.");
define("CMS_L_MOD_COMMENT_034", "Möchten Sie den Kommentar negativ bewerten?");
define("CMS_L_MOD_COMMENT_035", "Möchten Sie den Kommentar positiv bewerten?");
define("CMS_L_MOD_COMMENT_036", "Negativ bewerten");
define("CMS_L_MOD_COMMENT_037", "Positiv bewerten");
define("CMS_L_MOD_COMMENT_038", "Ihr Kommentar wurde als Spam erkannt.");
define("CMS_L_MOD_COMMENT_039", "Spam-Wahrscheinlichkeit");
define("CMS_L_MOD_COMMENT_040", "Ein identischer Kommentar wurde bereits abgegeben.");
define("CMS_L_MOD_COMMENT_041", "Freischaltung ihres Kommentars");
define("CMS_L_MOD_COMMENT_042", "Ihr Kommentar unter %s wurde soeben freigeschaltet.");
define("CMS_L_MOD_COMMENT_043", "CAPTCHA");
define("CMS_L_MOD_COMMENT_044", "Bitte geben Sie die Zeichen aus der Grafik ein.");
define("CMS_L_MOD_COMMENT_045", "Der CAPTCHA-Code ist ungültig.");

//mod_content
//..................................................................................................
define("CMS_L_MOD_CONTENT_002", "Der Zwischenspeicher ist leer.");
define("CMS_L_MOD_CONTENT_003", "Änderungen übernehmen");
define("CMS_L_MOD_CONTENT_004", "Änderungen verwerfen");
define("CMS_L_MOD_CONTENT_005", "Austauschen");
define("CMS_L_MOD_CONTENT_006", "Inhaltsanalyse");
define("CMS_L_MOD_CONTENT_007", "Backend");
define("CMS_L_MOD_CONTENT_008", "Desktop");
define("CMS_L_MOD_CONTENT_009", "Wiederholungen");
define("CMS_L_MOD_CONTENT_010", "Positionsänderung");
define("CMS_L_MOD_CONTENT_011", "Oben einfügen");
define("CMS_L_MOD_CONTENT_012", "Unten einfügen");
define("CMS_L_MOD_CONTENT_013", "Die Eingabe ist ungültig.");
define("CMS_L_MOD_CONTENT_014", "Sync-Quellen");
define("CMS_L_MOD_CONTENT_015", "Abmelden");
define("CMS_L_MOD_CONTENT_016", "Schritt zurück");
define("CMS_L_MOD_CONTENT_017", "Schritt vor");
define("CMS_L_MOD_CONTENT_018", "Metadaten");
define("CMS_L_MOD_CONTENT_020", "Debug-Info");
define("CMS_L_MOD_CONTENT_021", "Seitentemplate exportieren");

//mod_desktop
//..................................................................................................
define("CMS_L_MOD_DESKTOP_001", "Erstellen");
define("CMS_L_MOD_DESKTOP_002", "Nächste Termine");
define("CMS_L_MOD_DESKTOP_003", "Link");
define("CMS_L_MOD_DESKTOP_004", "Notiz");
define("CMS_L_MOD_DESKTOP_005", "Termin");
define("CMS_L_MOD_DESKTOP_006", "Kontakt");
define("CMS_L_MOD_DESKTOP_007", "Ordner");
define("CMS_L_MOD_DESKTOP_008", "E-Mail");
define("CMS_L_MOD_DESKTOP_009", "Umbenennen");
define("CMS_L_MOD_DESKTOP_010", "Es existieren keine Empfänger.");
define("CMS_L_MOD_DESKTOP_011", "Kopie an …");
define("CMS_L_MOD_DESKTOP_012", "Empfänger");
define("CMS_L_MOD_DESKTOP_013", "Legacy");
define("CMS_L_MOD_DESKTOP_014", "Aktuelles Passwort");
define("CMS_L_MOD_DESKTOP_015", "Neues Passwort");
define("CMS_L_MOD_DESKTOP_016", "Passwortwiederholung");
define("CMS_L_MOD_DESKTOP_017", "Hintergrundbild");
define("CMS_L_MOD_DESKTOP_018", "Favoriten");
define("CMS_L_MOD_DESKTOP_019", "Posteingang");
define("CMS_L_MOD_DESKTOP_020", "Messenger");
define("CMS_L_MOD_DESKTOP_021", "Adressbuch");
define("CMS_L_MOD_DESKTOP_022", "Terminkalender");
define("CMS_L_MOD_DESKTOP_023", "%s erstellen");
define("CMS_L_MOD_DESKTOP_024", "Home");
define("CMS_L_MOD_DESKTOP_025", "Zeitzone");
define("CMS_L_MOD_DESKTOP_026", "Einstellungen");
define("CMS_L_MOD_DESKTOP_027", "Passwort ändern");
define("CMS_L_MOD_DESKTOP_028", "Sonstige Einstellungen");
define("CMS_L_MOD_DESKTOP_029", "Backend");
define("CMS_L_MOD_DESKTOP_030", "Website");
define("CMS_L_MOD_DESKTOP_031", "Das aktuelle Passwort ist inkorrekt.");
define("CMS_L_MOD_DESKTOP_032", "Neues Passwort und Wiederholung stimmen nicht überein.");

//mod_download
//..................................................................................................
define("CMS_L_MOD_DOWNLOAD_001", "Download");
define("CMS_L_MOD_DOWNLOAD_002", "<a href=\"%s\">Klicken Sie hier</a>, " .
                                 "falls der Download nicht innerhalb von 3 Sekunden automatisch startet.");
define("CMS_L_MOD_DOWNLOAD_003", "Nach dem Beginn des Downloads können Sie dieses " .
                                 "<a href=\"javascript:window.close();\">Fenster schließen</a>.");
define("CMS_L_MOD_DOWNLOAD_007", "Die angeforderte Datei ist nicht verfügbar.");

//mod_forum
//..................................................................................................
define("CMS_L_MOD_FORUM_001", "%s schrieb am %s., den %s um %s");
define("CMS_L_MOD_FORUM_002", "AW: ");
define("CMS_L_MOD_FORUM_003", "Thema erstellen");
define("CMS_L_MOD_FORUM_004", "Suche");
define("CMS_L_MOD_FORUM_005", "Suchergebnisse");
define("CMS_L_MOD_FORUM_006", "Antworten");
define("CMS_L_MOD_FORUM_007", "Weitere Beiträge");
define("CMS_L_MOD_FORUM_008", "Ihre Suche ergab keine Ergebnisse.");
define("CMS_L_MOD_FORUM_009", "Per E-Mail über Antworten benachrichtigen");
define("CMS_L_MOD_FORUM_010", "Beitrag schreiben");
define("CMS_L_MOD_FORUM_011", "Soll der Beitrag gelöscht werden?");
define("CMS_L_MOD_FORUM_012", "Übersicht");
define("CMS_L_MOD_FORUM_013", "Der Beitrag konnte nicht gelöscht werden.");
define("CMS_L_MOD_FORUM_014", "Überschrift");
define("CMS_L_MOD_FORUM_015", "von %s");
define("CMS_L_MOD_FORUM_016", "%u Antworten");
define("CMS_L_MOD_FORUM_017", "Bitte geben Sie eine Überschrift und Inhalt ein.");
define("CMS_L_MOD_FORUM_018", "Der Beitrag konnte nicht bearbeitet werden.");
define("CMS_L_MOD_FORUM_019", "Der Beitrag wurde gelöscht.");
define("CMS_L_MOD_FORUM_020", "Der Benutzer existiert nicht oder hat keine Beiträge verfasst.");
define("CMS_L_MOD_FORUM_021", "Der Beiträg \"%s\" wird verschoben.");
define("CMS_L_MOD_FORUM_022", "Der Beitrag wurde erstellt.");
define("CMS_L_MOD_FORUM_023", "Der Beitrag konnte nicht erstellt werden.");
define("CMS_L_MOD_FORUM_024", "Der Beitrag wurde aktualisiert.");
define("CMS_L_MOD_FORUM_025", "Der Beitrag konnte nicht aktualisiert werden.");
define("CMS_L_MOD_FORUM_026", "Der Beitrag wurde verschoben.");
define("CMS_L_MOD_FORUM_027", "Der Beitrag konnte nicht verschoben werden.");
define("CMS_L_MOD_FORUM_028", "Inhalt");
define("CMS_L_MOD_FORUM_029", "Der angeforderte Beitrag ist nicht (mehr) verfügbar.");
define("CMS_L_MOD_FORUM_030", "Beitrag nicht gefunden");

//mod_identification
//..................................................................................................
define("CMS_L_MOD_IDENTIFICATION_001", "Sie sind momentan als %s (%s) angemeldet.");
define("CMS_L_MOD_IDENTIFICATION_002", "Benutzername");
define("CMS_L_MOD_IDENTIFICATION_003", "Abmelden");
define("CMS_L_MOD_IDENTIFICATION_004", "Anmeldung");
define("CMS_L_MOD_IDENTIFICATION_005", "Anmelden");
define("CMS_L_MOD_IDENTIFICATION_006", "Passwort vergessen?");
define("CMS_L_MOD_IDENTIFICATION_007", "Sie werden zur Anmeldung weitergeleitet. " .
                                       "<a href=\"%s\">Klicken Sie hier</a>, " .
                                       "wenn ihre Websoftware der Anforderung nicht entspricht.");
define("CMS_L_MOD_IDENTIFICATION_008", "Aktuellen Benutzer beibehalten");
define("CMS_L_MOD_IDENTIFICATION_009", "Neues Passwort setzen");
define("CMS_L_MOD_IDENTIFICATION_010", "Bitte geben Sie ihren Benutzernamen an.");
define("CMS_L_MOD_IDENTIFICATION_011", "Der angegebene Benutzer existiert nicht oder " .
                                       "verfügt über keine gültige E-Mail.");
define("CMS_L_MOD_IDENTIFICATION_012", "Es wurde eine Nachricht an die zugehörige E-Mail gesendet, " .
                                       "über die Sie ein neues Passwort setzen können.");
define("CMS_L_MOD_IDENTIFICATION_013", "Die Zeichen aus der Grafik stimmen nicht. " .
                                       "Bitte versuchen Sie es erneut.");
define("CMS_L_MOD_IDENTIFICATION_014", "CAPTCHA");
define("CMS_L_MOD_IDENTIFICATION_015", "Bitte geben Sie die Zeichen aus der Grafik ein.");
define("CMS_L_MOD_IDENTIFICATION_016", "Um das zugehörige Passwort des Benutzers %s auf %s zu überschreiben, " .
                                       "öffnen Sie die URL\n%s\ninnerhalb einer Stunde.\n\n" .
                                       "Sollten Sie kein neues Passwort angefordert haben, " .
                                       "können Sie diese Nachricht gefahrlos ignorieren.");
define("CMS_L_MOD_IDENTIFICATION_017", "Der Vorgang ist ungültig oder abgelaufen.");
define("CMS_L_MOD_IDENTIFICATION_018", "Es wurde ein neues Passwort gesetzt.");
define("CMS_L_MOD_IDENTIFICATION_019", "Das Passwort wurde an die zugehörige E-Mail gesendet.");
define("CMS_L_MOD_IDENTIFICATION_020", "Das Passwort konnte nicht an die zugehörige E-Mail gesendet werden.");
define("CMS_L_MOD_IDENTIFICATION_021", "Für den Benutzer %s auf %s wurde ein neues Passwort gesetzt.\n\n" .
                                       "\tPasswort:\t%s");
define("CMS_L_MOD_IDENTIFICATION_022", "Ihr neues Passwort");
define("CMS_L_MOD_IDENTIFICATION_023", "Legacy");
define("CMS_L_MOD_IDENTIFICATION_024", "Wegen zu vieler fehlgeschlagender Versuche " .
                                       "können Sie sich für die nächsten %u Minuten nicht mehr anmelden.");
define("CMS_L_MOD_IDENTIFICATION_025", "Sie haben noch %u verbleibende Versuche.");

//mod_image
//..................................................................................................
define("CMS_L_MOD_IMAGE_001", "Das Bild ist nicht verfügbar.");

//mod_mailform
//..................................................................................................
define("CMS_L_MOD_MAILFORM_001", "Sendebestätigung");
define("CMS_L_MOD_MAILFORM_002", "Das Formular wurde übermittelt.");
define("CMS_L_MOD_MAILFORM_003", "Abschicken");
define("CMS_L_MOD_MAILFORM_005", "Feld");
define("CMS_L_MOD_MAILFORM_006", "Angabe");
define("CMS_L_MOD_MAILFORM_008", "Bitte überprüfen Sie die markierten Angaben.");
define("CMS_L_MOD_MAILFORM_009", "CAPTCHA");
define("CMS_L_MOD_MAILFORM_010", "Bitte geben Sie die Zeichen aus der Grafik ein.");
define("CMS_L_MOD_MAILFORM_011", "Formulare");
define("CMS_L_MOD_MAILFORM_012", "Das Formular konnte nicht übermittelt werden.");

//mod_profile
//..................................................................................................
define("CMS_L_MOD_PROFILE_001", "(Keine Angabe)");
define("CMS_L_MOD_PROFILE_002", "Zugangsdaten");
define("CMS_L_MOD_PROFILE_003", "Personendaten");
define("CMS_L_MOD_PROFILE_004", "Kontaktdaten");
define("CMS_L_MOD_PROFILE_005", "Bankverbindung");
define("CMS_L_MOD_PROFILE_006", "Kreditkartendaten");
define("CMS_L_MOD_PROFILE_007", "Sonstige Daten");
define("CMS_L_MOD_PROFILE_008", "Kennung");
define("CMS_L_MOD_PROFILE_009", "Benutzername");
define("CMS_L_MOD_PROFILE_010", "Zusatzfeld");
define("CMS_L_MOD_PROFILE_011", "Erweiterte Informationen");
define("CMS_L_MOD_PROFILE_012", "Organisation");
define("CMS_L_MOD_PROFILE_013", "Vorname");
define("CMS_L_MOD_PROFILE_014", "Nachname");
define("CMS_L_MOD_PROFILE_015", "Straße");
define("CMS_L_MOD_PROFILE_016", "PLZ");
define("CMS_L_MOD_PROFILE_017", "Ort");
define("CMS_L_MOD_PROFILE_018", "Land");
define("CMS_L_MOD_PROFILE_019", "Telefon");
define("CMS_L_MOD_PROFILE_020", "Telefon 2");
define("CMS_L_MOD_PROFILE_021", "Mobiltelefon");
define("CMS_L_MOD_PROFILE_022", "Fax");
define("CMS_L_MOD_PROFILE_023", "Kontonummer");
define("CMS_L_MOD_PROFILE_024", "Geldinstitut");
define("CMS_L_MOD_PROFILE_025", "Bankleitzahl");
define("CMS_L_MOD_PROFILE_026", "Kontoinhaber");
define("CMS_L_MOD_PROFILE_027", "Kreditkartennummer");
define("CMS_L_MOD_PROFILE_028", "Kreditinstitut");
define("CMS_L_MOD_PROFILE_029", "Kreditkarteninhaber");
define("CMS_L_MOD_PROFILE_030", "Ablaufdatum");
define("CMS_L_MOD_PROFILE_031", "Anmerkung");
define("CMS_L_MOD_PROFILE_032", "Passwort und Wiederholung stimmen nicht überein.");
define("CMS_L_MOD_PROFILE_033", "Das Passwort muss mindestens 4 Zeichen enthalten.");
define("CMS_L_MOD_PROFILE_034", "Die Änderungen wurden gespeichert.");
define("CMS_L_MOD_PROFILE_035", "Passwortwiederholung");
define("CMS_L_MOD_PROFILE_036", "Bitte überprüfen Sie die markierten Angaben.");
define("CMS_L_MOD_PROFILE_037", "CAPTCHA");
define("CMS_L_MOD_PROFILE_038", "Bitte geben Sie die Zeichen aus der Grafik ein.");
define("CMS_L_MOD_PROFILE_039", "Registrierung");
define("CMS_L_MOD_PROFILE_040", "Konto erstellen");
define("CMS_L_MOD_PROFILE_041", "Bestätigung der Registrierung");
define("CMS_L_MOD_PROFILE_042", "Vielen Dank für ihre Registrierung auf %s.\n\n" .
                                "\tBenutzername:\t%s\n\tPasswort:\t%s\n\n" .
                                "Bitte bestätigen Sie ihre Anmeldung, indem Sie die URL\n%s\n" .
                                "innerhalb von 24 Stunden nach der Registrierung aufrufen.");
define("CMS_L_MOD_PROFILE_043", "Die Registrierung wurde durchgeführt. " .
                                "Sie erhalten nun eine Bestätigung per E-Mail, " .
                                "die den zur Freischaltung des Kontos benötigten " .
                                "Aktivierungscode enthält.");
define("CMS_L_MOD_PROFILE_044", "Bitte geben Sie eine gültige E-Mail an.");
define("CMS_L_MOD_PROFILE_045", "Der Benutzername ist bereits vergeben. " .
                                "Bitte wählen Sie eine Alternative.");
define("CMS_L_MOD_PROFILE_046", "Die Zeichen aus der Grafik stimmen nicht. " .
                                "Bitte versuchen Sie es erneut.");
define("CMS_L_MOD_PROFILE_047", "Der Aktivierungscode ist ungültig oder abgelaufen.");
define("CMS_L_MOD_PROFILE_048", "Ihr Benutzerkonto wurde aktiviert.");
define("CMS_L_MOD_PROFILE_049", "Lassen Sie das Feld leer, wenn Sie das Passwort nicht ändern möchten.");
define("CMS_L_MOD_PROFILE_051", "Das Feld darf nicht leer sein.");
define("CMS_L_MOD_PROFILE_052", "Es existiert bereits ein Konto unter dieser E-Mail.");
define("CMS_L_MOD_PROFILE_053", "Basisdaten");

//mod_profile_data
//..................................................................................................
define("CMS_L_MOD_PROFILE_DATA_001", "Telefon");
define("CMS_L_MOD_PROFILE_DATA_002", "Telefon 2");
define("CMS_L_MOD_PROFILE_DATA_003", "Mobiltelefon");
define("CMS_L_MOD_PROFILE_DATA_004", "Fax");

//mod_rss
//..................................................................................................
define("CMS_L_MOD_RSS_001", "Neu");
define("CMS_L_MOD_RSS_002", "Update");

//mod_search
//..................................................................................................
define("CMS_L_MOD_SEARCH_001", "Suche");
define("CMS_L_MOD_SEARCH_002", "Seite eintragen");
define("CMS_L_MOD_SEARCH_004", "Eintragen");
define("CMS_L_MOD_SEARCH_005", "Es wurden keine Ergebnisse für Ihre Suchanfrage gefunden.");
define("CMS_L_MOD_SEARCH_006", "Es gibt keine weiteren Ergebnisse für Ihre Suchanfrage.");
define("CMS_L_MOD_SEARCH_007", "Weitere Ergebnisse");
define("CMS_L_MOD_SEARCH_008", "%s | Stand: %s | Score: %s");
define("CMS_L_MOD_SEARCH_010", "Suche in %s Seiten");
define("CMS_L_MOD_SEARCH_011", " | %u ähnliche Seite(n)");
define("CMS_L_MOD_SEARCH_012", "Alle Sprachen");
define("CMS_L_MOD_SEARCH_013", "Suchbegriff");

//mod_security
//..................................................................................................
define("CMS_L_MOD_SECURITY_001", "Sicherheitsprüfung");
define("CMS_L_MOD_SECURITY_002", "Die Validierung für den folgenden Aufruf ist fehlgeschlagen:<br>" .
                                 "<br>" .
                                 "<code>%s</code><br>" .
                                 "<br>" .
                                 "Um Manipulationen (CSRF) zu verhindern, wurde die Aktion gestoppt.");
define("CMS_L_MOD_SECURITY_003", "Unbekannte Adresse");
define("CMS_L_MOD_SECURITY_004", "Zur Startseite");
define("CMS_L_MOD_SECURITY_005", "Ignorieren und fortsetzen");

//==================================================================================================
//   INTERFACE
//==================================================================================================

//ifc_content
//..................................................................................................
define("CMS_L_IFC_CONTENT",     "Redaktion");
define("CMS_L_IFC_CONTENT_001", "Ausführungszeit");
define("CMS_L_IFC_CONTENT_002", "Versandinfo");
define("CMS_L_IFC_CONTENT_003", "Absender");
define("CMS_L_IFC_CONTENT_004", "Datum");
define("CMS_L_IFC_CONTENT_005", "Vorgangsart");
define("CMS_L_IFC_CONTENT_006", "Status");
define("CMS_L_IFC_CONTENT_007", "Ausfertigung");
define("CMS_L_IFC_CONTENT_008", "Zusammenfassung");
define("CMS_L_IFC_CONTENT_009", "Editor");
define("CMS_L_IFC_CONTENT_010", "Stand");
define("CMS_L_IFC_CONTENT_011", "Navigation");
define("CMS_L_IFC_CONTENT_012", "Keywords");
define("CMS_L_IFC_CONTENT_013", "Seitentemplate");
define("CMS_L_IFC_CONTENT_014", "Autorisierung");
define("CMS_L_IFC_CONTENT_015", "Reviewer");
define("CMS_L_IFC_CONTENT_016", "Datum");
define("CMS_L_IFC_CONTENT_017", "Notiz");
define("CMS_L_IFC_CONTENT_018", "Veröffentlichung");
define("CMS_L_IFC_CONTENT_019", "Herausgeber");
define("CMS_L_IFC_CONTENT_020", "Datum");
define("CMS_L_IFC_CONTENT_021", "Entwurf anlegen");
define("CMS_L_IFC_CONTENT_022", "Meine Entwürfe");
define("CMS_L_IFC_CONTENT_023", "Einstellungen");
define("CMS_L_IFC_CONTENT_024", "Veröffentlichungen");
define("CMS_L_IFC_CONTENT_025", "Quelle anzeigen");
define("CMS_L_IFC_CONTENT_026", "(Leer)");
define("CMS_L_IFC_CONTENT_027", "Titel");
define("CMS_L_IFC_CONTENT_028", "Text");
define("CMS_L_IFC_CONTENT_029", "Autorisieren und sperren (Dokument)");
define("CMS_L_IFC_CONTENT_030", "Dokumente");
define("CMS_L_IFC_CONTENT_031", "Entwurf");
define("CMS_L_IFC_CONTENT_032", "Dokument");
define("CMS_L_IFC_CONTENT_033", "Veröffentlichung");
define("CMS_L_IFC_CONTENT_034", "Original");
define("CMS_L_IFC_CONTENT_035", "Bearbeitbare Kopie");
define("CMS_L_IFC_CONTENT_036", "Nur-Lesen-Kopie");
define("CMS_L_IFC_CONTENT_037", "Noindex");
define("CMS_L_IFC_CONTENT_038", "Nofollow");
define("CMS_L_IFC_CONTENT_039", "Fremde Entwürfe");
define("CMS_L_IFC_CONTENT_040", "Veröffentlichen");
define("CMS_L_IFC_CONTENT_041", "In Navigation verlinken …");
define("CMS_L_IFC_CONTENT_042", "Veröffentlichung");
define("CMS_L_IFC_CONTENT_043", "RSS-Channel");
define("CMS_L_IFC_CONTENT_044", "Funktion");
define("CMS_L_IFC_CONTENT_045", "Es existieren keine berechtigten Empfänger.");
define("CMS_L_IFC_CONTENT_046", "Weiterleiten");
define("CMS_L_IFC_CONTENT_047", "Verfasser");
define("CMS_L_IFC_CONTENT_048", "Empfänger");
define("CMS_L_IFC_CONTENT_049", "Entwurf erstellen");
define("CMS_L_IFC_CONTENT_050", "Content-Cache leeren");
define("CMS_L_IFC_CONTENT_051", "Bearbeitbare Kopie erstellen");
define("CMS_L_IFC_CONTENT_052", "Nur-Lesen-Kopie erstellen");
define("CMS_L_IFC_CONTENT_053", "Autorisierung und Sperrung aufheben (Entwurf)");
define("CMS_L_IFC_CONTENT_054", "Zurückziehen");
define("CMS_L_IFC_CONTENT_055", "Eingang");
define("CMS_L_IFC_CONTENT_056", "Meta Robots");
define("CMS_L_IFC_CONTENT_057", "Ausschließen");
define("CMS_L_IFC_CONTENT_058", "RSS-Channel von %s");
define("CMS_L_IFC_CONTENT_060", "Zeilen");
define("CMS_L_IFC_CONTENT_061", "RSS-Feed");
define("CMS_L_IFC_CONTENT_062", "Kategorie"); //rss
define("CMS_L_IFC_CONTENT_063", "Kategorie umbenennen");
define("CMS_L_IFC_CONTENT_064", "Markierte als Default setzen");
define("CMS_L_IFC_CONTENT_065", "ID");
define("CMS_L_IFC_CONTENT_066", "Statistik");
define("CMS_L_IFC_CONTENT_073", "Reset");
define("CMS_L_IFC_CONTENT_074", "Default auswählen");
define("CMS_L_IFC_CONTENT_075", "Markierte zuweisen");
define("CMS_L_IFC_CONTENT_076", "Zugewiesene Channels auswählen");
define("CMS_L_IFC_CONTENT_077", "Metadaten");
define("CMS_L_IFC_CONTENT_078", "RSS");
define("CMS_L_IFC_CONTENT_079", "RSS-Channel abonnieren");
define("CMS_L_IFC_CONTENT_080", "Quellcode");
define("CMS_L_IFC_CONTENT_084", "in Bearbeitung");
define("CMS_L_IFC_CONTENT_085", "Versionen");
define("CMS_L_IFC_CONTENT_086", "Aktivieren");
define("CMS_L_IFC_CONTENT_087", "Änderungen übernehmen");
define("CMS_L_IFC_CONTENT_088", "Änderungen verwerfen");
define("CMS_L_IFC_CONTENT_089", "Versionen");
define("CMS_L_IFC_CONTENT_090", "Vorschau nicht verfügbar. " .
                                "IFRAMEs sind deaktiviert oder werden von ihrer Websoftware nicht unterstützt.");
define("CMS_L_IFC_CONTENT_091", "Benutzerordner");
define("CMS_L_IFC_CONTENT_092", "Mappe");
define("CMS_L_IFC_CONTENT_093", "Filter");
define("CMS_L_IFC_CONTENT_094", "Alle");
define("CMS_L_IFC_CONTENT_095", "Alle Änderungen übernehmen");
define("CMS_L_IFC_CONTENT_096", "Textinhalt");
define("CMS_L_IFC_CONTENT_097", "Inhaltsanalyse");
define("CMS_L_IFC_CONTENT_098", "Wort");
define("CMS_L_IFC_CONTENT_099", "Vorkommen");
define("CMS_L_IFC_CONTENT_100", "Anteil");
define("CMS_L_IFC_CONTENT_101", "Gesamt");
define("CMS_L_IFC_CONTENT_102", "Wortspannweite");
define("CMS_L_IFC_CONTENT_103", "Einzelvorkommen werden nicht angezeigt.");
define("CMS_L_IFC_CONTENT_104", "HTML");
define("CMS_L_IFC_CONTENT_108", "Inhalts-ID");
define("CMS_L_IFC_CONTENT_109", "Sofort anwenden");
define("CMS_L_IFC_CONTENT_110", "Version speichern");
define("CMS_L_IFC_CONTENT_111", "Website-Link");
define("CMS_L_IFC_CONTENT_116", "Angaben zur Veröffentlichung");
define("CMS_L_IFC_CONTENT_117", "Nie anwenden");
define("CMS_L_IFC_CONTENT_118", "Erweitert");
define("CMS_L_IFC_CONTENT_119", "Gewählten Eintrag überschreiben");
define("CMS_L_IFC_CONTENT_120", "Zeitplan");
define("CMS_L_IFC_CONTENT_121", "Zusatzspalte");
define("CMS_L_IFC_CONTENT_122", "Version aktivieren");
define("CMS_L_IFC_CONTENT_123", "Zielpfad");
define("CMS_L_IFC_CONTENT_124", "Zielpfad wurde nicht angegeben oder ist ungültig.");
define("CMS_L_IFC_CONTENT_125", "Version");
define("CMS_L_IFC_CONTENT_126", "Version ist ungültig.");
define("CMS_L_IFC_CONTENT_127", "Aktion");
define("CMS_L_IFC_CONTENT_128", "Ohne Link veröffentlichen");
define("CMS_L_IFC_CONTENT_129", "Kategorie"); //pool
define("CMS_L_IFC_CONTENT_130", "Sync-Quellen");
define("CMS_L_IFC_CONTENT_131", "Sync-Quellen");
define("CMS_L_IFC_CONTENT_132", "Synchronisieren");
define("CMS_L_IFC_CONTENT_133", "Zwischenspeichern");
define("CMS_L_IFC_CONTENT_134", "Als Default für neue Seiten setzen");
define("CMS_L_IFC_CONTENT_135", "Version %u");
define("CMS_L_IFC_CONTENT_136", "Wert");
define("CMS_L_IFC_CONTENT_137", "Die gewählte Seite ist nicht verknüpft.");
define("CMS_L_IFC_CONTENT_138", "Info");
define("CMS_L_IFC_CONTENT_139", "XML-Sitemap");
define("CMS_L_IFC_CONTENT_140", "Einstellungen");
define("CMS_L_IFC_CONTENT_141", "bearbeitet");
define("CMS_L_IFC_CONTENT_142", "Erweitert");
define("CMS_L_IFC_CONTENT_143", "Alle Entwürfe");
define("CMS_L_IFC_CONTENT_144", "Bezeichnung");
define("CMS_L_IFC_CONTENT_145", "Seiten-ID");
define("CMS_L_IFC_CONTENT_146", "Debug-Info");

//ifc_database
//..................................................................................................
define("CMS_L_IFC_DATABASE",     "Datenbank");
define("CMS_L_IFC_DATABASE_001", "Spalte bearbeiten");
define("CMS_L_IFC_DATABASE_002", "Spalte erstellen");
define("CMS_L_IFC_DATABASE_003", "Datentyp");
define("CMS_L_IFC_DATABASE_004", "Typspezifische Attribute");
define("CMS_L_IFC_DATABASE_005", "Defaultwert");
define("CMS_L_IFC_DATABASE_006", "Größe");
define("CMS_L_IFC_DATABASE_007", "Attribut");
define("CMS_L_IFC_DATABASE_008", "Dezimalstellen");
define("CMS_L_IFC_DATABASE_009", "Format");
define("CMS_L_IFC_DATABASE_010", "Länge");
define("CMS_L_IFC_DATABASE_011", "Wertliste");
define("CMS_L_IFC_DATABASE_012", "Index erstellen");
define("CMS_L_IFC_DATABASE_013", "Speichersystem");
define("CMS_L_IFC_DATABASE_014", "Tabelle bearbeiten");
define("CMS_L_IFC_DATABASE_015", "Tabelle erstellen");
define("CMS_L_IFC_DATABASE_016", "Beschreibung");
define("CMS_L_IFC_DATABASE_017", "Dateneditor");
define("CMS_L_IFC_DATABASE_018", "Anzeige");
define("CMS_L_IFC_DATABASE_021", "Zeilen");
define("CMS_L_IFC_DATABASE_022", "Zeichen");
define("CMS_L_IFC_DATABASE_023", "Filter");
define("CMS_L_IFC_DATABASE_024", "Enthält");
define("CMS_L_IFC_DATABASE_025", "Sortierung");
define("CMS_L_IFC_DATABASE_026", "%s Datensätze");
define("CMS_L_IFC_DATABASE_027", "Export-Dateiname");
define("CMS_L_IFC_DATABASE_028", "Spaltentrennzeichen");
define("CMS_L_IFC_DATABASE_029", "Komma (MS Excel)");
define("CMS_L_IFC_DATABASE_030", "Semikolon (MS Access)");
define("CMS_L_IFC_DATABASE_031", "Tab");
define("CMS_L_IFC_DATABASE_032", "Leerzeichen");
define("CMS_L_IFC_DATABASE_033", "Wertbegrenzer");
define("CMS_L_IFC_DATABASE_034", "\" (MS Excel, MS Access)");
define("CMS_L_IFC_DATABASE_035", "'");
define("CMS_L_IFC_DATABASE_036", "(Kein)");
define("CMS_L_IFC_DATABASE_037", "Daten exportieren");
define("CMS_L_IFC_DATABASE_038", "Daten importieren");
define("CMS_L_IFC_DATABASE_039", "Importdatei");
define("CMS_L_IFC_DATABASE_040", "Es besteht keine Verbindung.");
define("CMS_L_IFC_DATABASE_041", "MySQL-Server");
define("CMS_L_IFC_DATABASE_042", "MySQL-Datenbank");
define("CMS_L_IFC_DATABASE_043", "Benutzername");
define("CMS_L_IFC_DATABASE_044", "Passwort");
define("CMS_L_IFC_DATABASE_045", "Datensatz hinzufügen");
define("CMS_L_IFC_DATABASE_046", "Vorhandene Datensätze überspringen");
define("CMS_L_IFC_DATABASE_047", "Exportdatei herunterladen");
define("CMS_L_IFC_DATABASE_048", "CSV-Einstellungen");
define("CMS_L_IFC_DATABASE_050", "Verbindung einrichten");
define("CMS_L_IFC_DATABASE_051", "SQL-Konsole");
define("CMS_L_IFC_DATABASE_052", "SQL-Anweisung");
define("CMS_L_IFC_DATABASE_053", "Datenbank warten");
define("CMS_L_IFC_DATABASE_054", "Tabelle");
define("CMS_L_IFC_DATABASE_056", "Datensatzformat");
define("CMS_L_IFC_DATABASE_057", "Anzahl Datensätze");
define("CMS_L_IFC_DATABASE_058", "Durchschn. Datensatzgröße");
define("CMS_L_IFC_DATABASE_059", "Datengröße");
define("CMS_L_IFC_DATABASE_060", "Maximale Datengröße");
define("CMS_L_IFC_DATABASE_061", "Indexgröße");
define("CMS_L_IFC_DATABASE_062", "Freier reservierter Speicher");
define("CMS_L_IFC_DATABASE_063", "Nächster Autowert");
define("CMS_L_IFC_DATABASE_064", "Erstellt");
define("CMS_L_IFC_DATABASE_065", "Geändert");
define("CMS_L_IFC_DATABASE_066", "Überprüft");
define("CMS_L_IFC_DATABASE_067", "NULL-Wert");
define("CMS_L_IFC_DATABASE_068", "Index");
define("CMS_L_IFC_DATABASE_069", "Optionen");
define("CMS_L_IFC_DATABASE_070", "Eindeutige Werte");
define("CMS_L_IFC_DATABASE_071", "Indexposition");
define("CMS_L_IFC_DATABASE_073", "Anzahl eindeutiger Werte");
define("CMS_L_IFC_DATABASE_074", "Struktur verwenden");
define("CMS_L_IFC_DATABASE_075", "Struktur speichern");
define("CMS_L_IFC_DATABASE_077", "Erste Zeile enthält Spaltennamen");
define("CMS_L_IFC_DATABASE_079", "(Leer)");
define("CMS_L_IFC_DATABASE_080", "Enthält nicht");
define("CMS_L_IFC_DATABASE_081", "Ausführen");
define("CMS_L_IFC_DATABASE_082", "Datenbank sichern");
define("CMS_L_IFC_DATABASE_083", "Datenbank wiederherstellen");
define("CMS_L_IFC_DATABASE_084", "Format");
define("CMS_L_IFC_DATABASE_085", "Komprimierung");
define("CMS_L_IFC_DATABASE_086", "Version");
define("CMS_L_IFC_DATABASE_087", "Prüfsumme");
define("CMS_L_IFC_DATABASE_088", "Zeichensatzsortierung");
define("CMS_L_IFC_DATABASE_089", "Berechtigung");
define("CMS_L_IFC_DATABASE_090", "(Default)");
define("CMS_L_IFC_DATABASE_091", "Indextyp");
define("CMS_L_IFC_DATABASE_092", "Einbezogene Spalten");
define("CMS_L_IFC_DATABASE_093", "Spalte");
define("CMS_L_IFC_DATABASE_094", "Inhalt");
define("CMS_L_IFC_DATABASE_095", "Spaltenzuordnung");

//ifc_directory
//..................................................................................................
define("CMS_L_IFC_DIRECTORY",     "Navigation");
define("CMS_L_IFC_DIRECTORY_001", "Neuer Link");
define("CMS_L_IFC_DIRECTORY_002", "Ungenutzte Links entfernen");
define("CMS_L_IFC_DIRECTORY_003", "Schritt 1: Einstellungen");
define("CMS_L_IFC_DIRECTORY_004", "Schritt 2: Position");
define("CMS_L_IFC_DIRECTORY_005", "Versteckt");
define("CMS_L_IFC_DIRECTORY_006", "Hierhin ziehen, um zu löschen");
define("CMS_L_IFC_DIRECTORY_007", "Einstellungen");
define("CMS_L_IFC_DIRECTORY_008", "Linktitel");
define("CMS_L_IFC_DIRECTORY_009", "Aktiv (Bild/URL)");
define("CMS_L_IFC_DIRECTORY_010", "Leer verstecken");
define("CMS_L_IFC_DIRECTORY_011", "Grafik");
define("CMS_L_IFC_DIRECTORY_012", "Normal (Bild/URL)");
define("CMS_L_IFC_DIRECTORY_013", "Hover (Bild/URL)");
define("CMS_L_IFC_DIRECTORY_014", "Verknüpfung");
define("CMS_L_IFC_DIRECTORY_015", "Pfad festlegen");
define("CMS_L_IFC_DIRECTORY_016", "Automatisch löschen");
define("CMS_L_IFC_DIRECTORY_017", "Neues Icon");
define("CMS_L_IFC_DIRECTORY_018", "Linkbeschreibung");
define("CMS_L_IFC_DIRECTORY_019", "Daten");
define("CMS_L_IFC_DIRECTORY_021", "Kanonische Basis-URL");
define("CMS_L_IFC_DIRECTORY_022", "Kanonische URL");
define("CMS_L_IFC_DIRECTORY_062", "Icons");
define("CMS_L_IFC_DIRECTORY_063", "Icon");
define("CMS_L_IFC_DIRECTORY_064", "Normale Grafik");
define("CMS_L_IFC_DIRECTORY_065", "Aktive Grafik");
define("CMS_L_IFC_DIRECTORY_072", "Alphabetisch sortieren");

//ifc_download
//..................................................................................................
define("CMS_L_IFC_DOWNLOAD",     "Downloads");
define("CMS_L_IFC_DOWNLOAD_001", "Datei hochladen");
define("CMS_L_IFC_DOWNLOAD_002", "Datei");
define("CMS_L_IFC_DOWNLOAD_003", "Kategorie");
define("CMS_L_IFC_DOWNLOAD_004", "Vorschau nicht verfügbar. " .
                                 "IFRAMEs sind deaktiviert oder werden von ihrer Websoftware nicht unterstützt.");
define("CMS_L_IFC_DOWNLOAD_005", "Dateiinfo");
define("CMS_L_IFC_DOWNLOAD_006", "Kategorie umbenennen");
define("CMS_L_IFC_DOWNLOAD_007", "Downloads");
define("CMS_L_IFC_DOWNLOAD_008", "Datei austauschen");
define("CMS_L_IFC_DOWNLOAD_011", "Format");
define("CMS_L_IFC_DOWNLOAD_012", "Größe");
define("CMS_L_IFC_DOWNLOAD_013", "Download");
define("CMS_L_IFC_DOWNLOAD_014", "Bulk-Upload");
define("CMS_L_IFC_DOWNLOAD_015", "Hier können Sie mehrere Dateien zum Upload auswählen.");

//ifc_easypage
//..................................................................................................
define("CMS_L_IFC_EASYPAGE",     "Webseiten");
define("CMS_L_IFC_EASYPAGE_001", "Navigation");
define("CMS_L_IFC_EASYPAGE_002", "Einstellungen");
define("CMS_L_IFC_EASYPAGE_003", "Schritt 1: Einstellungen");
define("CMS_L_IFC_EASYPAGE_004", "Hierhin ziehen, um zu löschen");
define("CMS_L_IFC_EASYPAGE_005", "Keywords");
define("CMS_L_IFC_EASYPAGE_006", "Schritt 2: Position");
define("CMS_L_IFC_EASYPAGE_007", "Neue Webseite");
define("CMS_L_IFC_EASYPAGE_008", "Hier legen Sie die Daten der Webseite fest.");
define("CMS_L_IFC_EASYPAGE_009", "Linktitel");
define("CMS_L_IFC_EASYPAGE_010", "Wählen Sie die Seite, die Sie bearbeiten möchten.");
define("CMS_L_IFC_EASYPAGE_011", "Versteckt");
define("CMS_L_IFC_EASYPAGE_013", "Seitentitel");
define("CMS_L_IFC_EASYPAGE_014", "Metadaten");
define("CMS_L_IFC_EASYPAGE_015", "Seitentemplate");
define("CMS_L_IFC_EASYPAGE_021", "Wählen Sie die Position, an der die neue Webseite erscheinen soll.");
define("CMS_L_IFC_EASYPAGE_022", "Hier legen Sie den Titel und das Design der neuen Webseite fest.");

//ifc_filemanager
//..................................................................................................
define("CMS_L_IFC_FILEMANAGER",     "Dateien");
define("CMS_L_IFC_FILEMANAGER_001", "Upload");
define("CMS_L_IFC_FILEMANAGER_002", "Datei-Upload");
define("CMS_L_IFC_FILEMANAGER_003", "Dateiinhalt");
define("CMS_L_IFC_FILEMANAGER_004", "Dateiname");
define("CMS_L_IFC_FILEMANAGER_005", "Kopieren nach …");
define("CMS_L_IFC_FILEMANAGER_006", "Ordner erstellen");
define("CMS_L_IFC_FILEMANAGER_007", "Datei erstellen");
define("CMS_L_IFC_FILEMANAGER_008", "Link zu Navigation hinzufügen");
define("CMS_L_IFC_FILEMANAGER_009", "Einstellungen");
define("CMS_L_IFC_FILEMANAGER_010", "Dateirechte");
define("CMS_L_IFC_FILEMANAGER_011", "Besitzer");
define("CMS_L_IFC_FILEMANAGER_012", "Gruppe");
define("CMS_L_IFC_FILEMANAGER_013", "Jeder");
define("CMS_L_IFC_FILEMANAGER_014", "Lesen");
define("CMS_L_IFC_FILEMANAGER_015", "Schreiben");
define("CMS_L_IFC_FILEMANAGER_016", "Ausführen");
define("CMS_L_IFC_FILEMANAGER_017", "Schreibschutz (Windows)");
define("CMS_L_IFC_FILEMANAGER_018", "Typ");
define("CMS_L_IFC_FILEMANAGER_019", "Größe");
define("CMS_L_IFC_FILEMANAGER_020", "Geändert");
define("CMS_L_IFC_FILEMANAGER_021", "Letzter Zugriff");
define("CMS_L_IFC_FILEMANAGER_022", "Verfügbarer Speicherplatz");
define("CMS_L_IFC_FILEMANAGER_023", "Downloadzeit (10 Mbit/s)");
define("CMS_L_IFC_FILEMANAGER_024", "Design");
define("CMS_L_IFC_FILEMANAGER_025", "Daten");
define("CMS_L_IFC_FILEMANAGER_026", "Datenbank");
define("CMS_L_IFC_FILEMANAGER_027", "Ziehen Sie Dateien zum Hochladen auf das obige Feld.");
define("CMS_L_IFC_FILEMANAGER_028", "Logs");
define("CMS_L_IFC_FILEMANAGER_029", "Download");
define("CMS_L_IFC_FILEMANAGER_030", "Verschieben nach …");
define("CMS_L_IFC_FILEMANAGER_031", "Libraries");
define("CMS_L_IFC_FILEMANAGER_032", "3rd-Party");
define("CMS_L_IFC_FILEMANAGER_033", "Übertragen von …");
define("CMS_L_IFC_FILEMANAGER_034", "Lokalisierung");
define("CMS_L_IFC_FILEMANAGER_035", "Sicherung");
define("CMS_L_IFC_FILEMANAGER_036", "GZip");
define("CMS_L_IFC_FILEMANAGER_037", "Un-GZip");
define("CMS_L_IFC_FILEMANAGER_038", "Gesamter Speicherplatz");
define("CMS_L_IFC_FILEMANAGER_039", "MIME-Typ");
define("CMS_L_IFC_FILEMANAGER_040", "Datei-Info");

//ifc_image
//..................................................................................................
define("CMS_L_IFC_IMAGE",     "Bilder");
define("CMS_L_IFC_IMAGE_001", "Bild hochladen");
define("CMS_L_IFC_IMAGE_002", "Bilddatei");
define("CMS_L_IFC_IMAGE_003", "Einstellungen");
define("CMS_L_IFC_IMAGE_004", "Vorschau nicht verfügbar. " .
                              "IFRAMEs sind deaktiviert oder werden von ihrer Websoftware nicht unterstützt.");
define("CMS_L_IFC_IMAGE_005", "Bilder im Hintergrund verarbeiten");
define("CMS_L_IFC_IMAGE_006", "Bevorzugtes Bildformat");
define("CMS_L_IFC_IMAGE_007", "Bilder");
define("CMS_L_IFC_IMAGE_008", "Bild austauschen");
define("CMS_L_IFC_IMAGE_009", "URL hinzufügen");
define("CMS_L_IFC_IMAGE_010", "Kategorie umbenennen");
define("CMS_L_IFC_IMAGE_011", "Bild-Cache leeren");
define("CMS_L_IFC_IMAGE_012", "Die Quelle ist extern und kann nicht intern ersetzt werden. " .
                              "Wählen Sie <em>Bearbeiten</em>, um die Quelle zu ändern.");
define("CMS_L_IFC_IMAGE_013", "Dateiname");
define("CMS_L_IFC_IMAGE_014", "Kategorie");
define("CMS_L_IFC_IMAGE_015", "Erweiterte Einstellungen");
define("CMS_L_IFC_IMAGE_016", "Bulk-Upload");
define("CMS_L_IFC_IMAGE_017", "Hier können Sie mehrere Dateien zum Upload auswählen.");
define("CMS_L_IFC_IMAGE_018", "Maximale Bildauflösung");

//ifc_insert
//..................................................................................................
define("CMS_L_IFC_INSERT",     "Einblendungen");
define("CMS_L_IFC_INSERT_002", "Neue Einblendung");
define("CMS_L_IFC_INSERT_003", "Einblendung");
define("CMS_L_IFC_INSERT_004", "Einblendungen");
define("CMS_L_IFC_INSERT_005", "Einstellungen");
define("CMS_L_IFC_INSERT_006", "HTML");
define("CMS_L_IFC_INSERT_007", "Inhalt");
define("CMS_L_IFC_INSERT_008", "Position");
//ifc_language
//..................................................................................................

define("CMS_L_IFC_LANGUAGE",     "Sprachen");
define("CMS_L_IFC_LANGUAGE_004", "IETF-Code");
define("CMS_L_IFC_LANGUAGE_005", "Sprachname");
define("CMS_L_IFC_LANGUAGE_006", "Einstellungen");
define("CMS_L_IFC_LANGUAGE_007", "Icon");
define("CMS_L_IFC_LANGUAGE_009", "Markierte aktivieren");
define("CMS_L_IFC_LANGUAGE_010", "Neue Sprache");
define("CMS_L_IFC_LANGUAGE_011", "Aktive auswählen");
define("CMS_L_IFC_LANGUAGE_012", "Stoppwörter (Whitespace-getrennt)");
define("CMS_L_IFC_LANGUAGE_013", "Defaultsprache");

//ifc_log
//..................................................................................................
define("CMS_L_IFC_LOG",     "Log");
define("CMS_L_IFC_LOG_001", "Datenumfang");
define("CMS_L_IFC_LOG_002", "Unbegrenzt");
define("CMS_L_IFC_LOG_003", "3 Jahre");
define("CMS_L_IFC_LOG_004", "2 Jahre");
define("CMS_L_IFC_LOG_005", "1 Jahr");
define("CMS_L_IFC_LOG_006", "6 Monate");
define("CMS_L_IFC_LOG_007", "3 Monate");
define("CMS_L_IFC_LOG_008", "1 Monat");
define("CMS_L_IFC_LOG_009", "2 Wochen");
define("CMS_L_IFC_LOG_010", "1 Woche");
define("CMS_L_IFC_LOG_011", "3 Tage");
define("CMS_L_IFC_LOG_012", "2 Tage");
define("CMS_L_IFC_LOG_013", "1 Tag");
define("CMS_L_IFC_LOG_014", "Deaktiviert");
define("CMS_L_IFC_LOG_015", "IP anonymisieren");
define("CMS_L_IFC_LOG_016", "Keine Benutzerdaten speichern");
define("CMS_L_IFC_LOG_017", "Weitere anzeigen");
define("CMS_L_IFC_LOG_018", "Tag");
define("CMS_L_IFC_LOG_019", "Woche");
define("CMS_L_IFC_LOG_020", "Monat");
define("CMS_L_IFC_LOG_021", "Quartal");
define("CMS_L_IFC_LOG_022", "Halbjahr");
define("CMS_L_IFC_LOG_023", "Jahr");
define("CMS_L_IFC_LOG_024", "2 Jahre");
define("CMS_L_IFC_LOG_025", "3 Jahre");
define("CMS_L_IFC_LOG_026", "4 Jahre");
define("CMS_L_IFC_LOG_027", "5 Jahre");
define("CMS_L_IFC_LOG_028", "Dekade");
define("CMS_L_IFC_LOG_029", "User-ID");
define("CMS_L_IFC_LOG_030", "Benutzer");
define("CMS_L_IFC_LOG_031", "IP");
define("CMS_L_IFC_LOG_032", "Aktion");
define("CMS_L_IFC_LOG_033", "Pfad");
define("CMS_L_IFC_LOG_034", "URL");
define("CMS_L_IFC_LOG_035", "Ref.-Domain");
define("CMS_L_IFC_LOG_036", "Ref.-URL");
define("CMS_L_IFC_LOG_037", "Client");
define("CMS_L_IFC_LOG_038", "User-Agent");
define("CMS_L_IFC_LOG_039", "Sprache");
define("CMS_L_IFC_LOG_040", "Region");
define("CMS_L_IFC_LOG_041", "Enthält");
define("CMS_L_IFC_LOG_042", "Nicht");
define("CMS_L_IFC_LOG_043", "Ist");
define("CMS_L_IFC_LOG_044", "Einstellungen");
define("CMS_L_IFC_LOG_045", "Enddatum");
define("CMS_L_IFC_LOG_046", "Zeitraum");
define("CMS_L_IFC_LOG_047", "Filter");
define("CMS_L_IFC_LOG_048", "Bots");
define("CMS_L_IFC_LOG_049", "Aus");
define("CMS_L_IFC_LOG_050", "Nur");
define("CMS_L_IFC_LOG_051", "Alles");
define("CMS_L_IFC_LOG_052", "Reset");
define("CMS_L_IFC_LOG_053", "Zugriffe");
define("CMS_L_IFC_LOG_054", "Graph");
define("CMS_L_IFC_LOG_055", "Verweisquellen");
define("CMS_L_IFC_LOG_056", "Besuchte Seiten");
define("CMS_L_IFC_LOG_057", "Top-Aktionen");
define("CMS_L_IFC_LOG_058", "Clients"); //plural
define("CMS_L_IFC_LOG_059", "Bevorzugte Sprachen");
define("CMS_L_IFC_LOG_060", "Regionen");
define("CMS_L_IFC_LOG_061", "Aktivste Nutzer");
define("CMS_L_IFC_LOG_062", "Aktivität je Tageszeit");
define("CMS_L_IFC_LOG_063", "Pseudonym");
define("CMS_L_IFC_LOG_064", "Info");
define("CMS_L_IFC_LOG_065", "Status");
define("CMS_L_IFC_LOG_066", "Details");
define("CMS_L_IFC_LOG_067", "Datum");
define("CMS_L_IFC_LOG_068", "Uhrzeit");
define("CMS_L_IFC_LOG_069", "Verweisquelle");
define("CMS_L_IFC_LOG_070", "Anzahl");
define("CMS_L_IFC_LOG_071", "Mobil");
define("CMS_L_IFC_LOG_072", "Status");
define("CMS_L_IFC_LOG_073", "Benutzer (festgelegt)");
define("CMS_L_IFC_LOG_074", "Benutzer (vorläufig)");
define("CMS_L_IFC_LOG_075", "Bot (vorläufig)");
define("CMS_L_IFC_LOG_076", "Bot (Limit überschritten)");
define("CMS_L_IFC_LOG_077", "Bot (festgelegt)");
define("CMS_L_IFC_LOG_078", "Bad Bot");
define("CMS_L_IFC_LOG_079", "Erkennung von und Umgang mit Bots");
define("CMS_L_IFC_LOG_080", "Bot-Erkennungsgrenze");
define("CMS_L_IFC_LOG_081", "Dies ist die Anzahl an Zugriffen pro 10 Sekunden, " .
                            "welche die Bot-Erkennung auslöst.");
define("CMS_L_IFC_LOG_082", "Bad-Bot-Erkennungsgrenze");
define("CMS_L_IFC_LOG_083", "Dies ist die Anzahl ungültiger Zugriffe pro 10 Sekunden, " .
                            "welche die Bad-Bot-Erkennung auslöst. " .
                            "WARNUNG: Zu aggressive Grenzwerte können Suchmaschinen-Crawler erfassen, " .
                            "was zum Ausschluss aus Suchmaschinen führt, wenn die Gegenmaßnahme aktiv ist.");
define("CMS_L_IFC_LOG_084", "Gegenmaßnahme-Verzögerung");
define("CMS_L_IFC_LOG_085", "Dies setzt die Verzögerung nach der Bad-Bot-Erkennung, " .
                            "bevor die Gegenmaßnahme einsetzt. " .
                            "Es verhindert, dass Bots die Erkennungsgrenze lernen und umgehen.");
define("CMS_L_IFC_LOG_086", "Bad-Bot-Gegenmaßnahme aktivieren");
define("CMS_L_IFC_LOG_087", "Bricht Anfragen von Bad Bots ohne Ausgabe ab, " .
                            "spart Ressourcen und verhindert Informationslecks.");
define("CMS_L_IFC_LOG_088", "Datenschutz");
define("CMS_L_IFC_LOG_089", "%u Zugriffe / 10 Sekunden");
define("CMS_L_IFC_LOG_090", "%u Sekunden");
define("CMS_L_IFC_LOG_091", "Fehlerseite");
define("CMS_L_IFC_LOG_092", "Bot-Status-Resetintervall");
define("CMS_L_IFC_LOG_093", "Dies ist der Zeitraum nach dem letzten Zugriff, " .
                            "nach dem der Bot-Status auf &quot;Bot (vorläufig)&quot; zurückgesetzt wird, " .
                            "wenn der Status nicht festgelegt ist.");
define("CMS_L_IFC_LOG_094", "Herkunft");
define("CMS_L_IFC_LOG_095", "Wird geladen …");
define("CMS_L_IFC_LOG_096", "Aktivität");
define("CMS_L_IFC_LOG_097", "Region");
define("CMS_L_IFC_LOG_098", "Technologie");
define("CMS_L_IFC_LOG_099", "Identität");
define("CMS_L_IFC_LOG_100", "Inhalt");
define("CMS_L_IFC_LOG_101", "Uhrzeit");
define("CMS_L_IFC_LOG_102", "Sprache");
define("CMS_L_IFC_LOG_103", "Bot-Datenumfang");
define("CMS_L_IFC_LOG_104", "Dies ist der Zeitraum, für den Bot-Zugriffsdaten vorgehalten werden.");

//ifc_mailform
//..................................................................................................
define("CMS_L_IFC_MAILFORM",     "Formulare");
define("CMS_L_IFC_MAILFORM_001", "Neues Formular");
define("CMS_L_IFC_MAILFORM_002", "Element erstellen");
define("CMS_L_IFC_MAILFORM_003", "Typ");
define("CMS_L_IFC_MAILFORM_004", "Optionen");
define("CMS_L_IFC_MAILFORM_005", "Auswahl");
define("CMS_L_IFC_MAILFORM_006", "Auswahlliste");
define("CMS_L_IFC_MAILFORM_007", "Textzeile");
define("CMS_L_IFC_MAILFORM_008", "Textfeld");
define("CMS_L_IFC_MAILFORM_009", "Neues Element");
define("CMS_L_IFC_MAILFORM_010", "Formular erstellen");
define("CMS_L_IFC_MAILFORM_011", "Alle");
define("CMS_L_IFC_MAILFORM_012", "Einstellungen");
define("CMS_L_IFC_MAILFORM_013", "Empfänger E-Mail");
define("CMS_L_IFC_MAILFORM_014", "Optionen");
define("CMS_L_IFC_MAILFORM_015", "Setzen Sie + an den Zeilenanfang, um eine Option vorzugeben.");
define("CMS_L_IFC_MAILFORM_016", "Breite");
define("CMS_L_IFC_MAILFORM_017", "Zeichen");
define("CMS_L_IFC_MAILFORM_018", "Default");
define("CMS_L_IFC_MAILFORM_019", "Eingabeprüfung");
define("CMS_L_IFC_MAILFORM_020", "Name");
define("CMS_L_IFC_MAILFORM_021", "E-Mail");
define("CMS_L_IFC_MAILFORM_022", "Telefon/Fax");
define("CMS_L_IFC_MAILFORM_023", "Eingabe erforderlich");
define("CMS_L_IFC_MAILFORM_024", "Anzahl Zeilen");
define("CMS_L_IFC_MAILFORM_025", "Beschreibung");
define("CMS_L_IFC_MAILFORM_026", "Bestätigungstext");
define("CMS_L_IFC_MAILFORM_027", "Ziel-URL (HTTP POST)");
define("CMS_L_IFC_MAILFORM_028", "Wenn keine Feld-ID gesetzt ist, " .
                                 "werden die Feldeingaben bei der Übertragung per HTTP POST unter " .
                                 "<strong>mailform_value(n)</strong> in Feldreihenfolge übermittelt: " .
                                 "<strong>mailform_value1</strong>, <strong>mailform_value2</strong>, etc.<br>" .
                                 "<br>" .
                                 "Die ID der Betreffzeile lautet <strong>mailform_subject</strong>.");
define("CMS_L_IFC_MAILFORM_029", "Verstecktes Feld");
define("CMS_L_IFC_MAILFORM_030", "Feld-ID");
define("CMS_L_IFC_MAILFORM_031", "Zahl");
define("CMS_L_IFC_MAILFORM_032", "Enthält E-Mail, Bestätigung übermitteln");
define("CMS_L_IFC_MAILFORM_033", "Seitenumbruch");
define("CMS_L_IFC_MAILFORM_034", "Anzahl Spalten");
define("CMS_L_IFC_MAILFORM_035", "Eigene Prüfung (PCRE)");
define("CMS_L_IFC_MAILFORM_036", "Umkehren");
define("CMS_L_IFC_MAILFORM_037", "Keine");
define("CMS_L_IFC_MAILFORM_038", "Schritt 1: Einstellungen");
define("CMS_L_IFC_MAILFORM_039", "Schritt 2: Position");
define("CMS_L_IFC_MAILFORM_040", "Verbindung einrichten");
define("CMS_L_IFC_MAILFORM_041", "SMTP-Server");
define("CMS_L_IFC_MAILFORM_042", "E-Mail (From)");
define("CMS_L_IFC_MAILFORM_043", "Benutzername");
define("CMS_L_IFC_MAILFORM_044", "Passwort");
define("CMS_L_IFC_MAILFORM_045", "Testmail senden");
define("CMS_L_IFC_MAILFORM_046", "CAPTCHA verwenden");
define("CMS_L_IFC_MAILFORM_047", "HTML-Code");
define("CMS_L_IFC_MAILFORM_048", "HTML");
define("CMS_L_IFC_MAILFORM_049", "Der HTML-Code muss ein Formularfeld enthalten, " .
                                 "dessen NAME-Attribut der Feld-ID entspricht.<br>" .
                                 "<br>" .
                                 "Verfügbare Platzhalter: " .
                                 "<strong>%label%</strong> Feldbezeichnung, " .
                                 "<strong>%id%</strong> Feld-ID, " .
                                 "<strong>%name%</strong> Feldname, " .
                                 "<strong>%value%</strong> Feldwert");
define("CMS_L_IFC_MAILFORM_050", "Submit-Button-Beschriftung");
define("CMS_L_IFC_MAILFORM_051", "Nur Code anzeigen");
define("CMS_L_IFC_MAILFORM_052", "mail()-Funktion verwenden");
define("CMS_L_IFC_MAILFORM_053", "SMTP-Server verwenden");
define("CMS_L_IFC_MAILFORM_054", "SMTP-Einstellungen");
define("CMS_L_IFC_MAILFORM_055", "[TEST] Automatisch generierte Testnachricht");
define("CMS_L_IFC_MAILFORM_056", "Nicht in Bestätigung anzeigen");
define("CMS_L_IFC_MAILFORM_057", "URL");
define("CMS_L_IFC_MAILFORM_058", "Straße/Nr.");
define("CMS_L_IFC_MAILFORM_059", "Hierhin ziehen, um zu löschen");
define("CMS_L_IFC_MAILFORM_060", "Dies ist eine automatisch generierte Testnachricht, " .
                                 "um die Funktion des E-Mail-Versands zu prüfen. " .
                                 "Keine Aktion Ihrerseits ist erforderlich. " .
                                 "Sie können diese Nachricht bedenkenlos löschen. " .
                                 "Der Erhalt dieser Mail zeigt, " .
                                 "dass der Versand korrekt ausgeführt wird.");
define("CMS_L_IFC_MAILFORM_061", "E-Mail (Reply-To)");

//ifc_media
//..................................................................................................
define("CMS_L_IFC_MEDIA",     "Media");
define("CMS_L_IFC_MEDIA_001", "Media hochladen");
define("CMS_L_IFC_MEDIA_002", "Mediadatei");
define("CMS_L_IFC_MEDIA_003", "URL hinzufügen");
define("CMS_L_IFC_MEDIA_004", "Vorschau nicht verfügbar. " .
                              "IFRAMEs sind deaktiviert oder werden von ihrer Websoftware nicht unterstützt.");
define("CMS_L_IFC_MEDIA_005", "Kategorie umbenennen");
define("CMS_L_IFC_MEDIA_007", "Media");
define("CMS_L_IFC_MEDIA_008", "Media austauschen");
define("CMS_L_IFC_MEDIA_010", "Mediaformate");
define("CMS_L_IFC_MEDIA_011", "Neues Mediaformat");
define("CMS_L_IFC_MEDIA_012", "Mediaformat");
define("CMS_L_IFC_MEDIA_013", "Zugeordnete Dateiendungen (kommagetrennt)");
define("CMS_L_IFC_MEDIA_014", "Einbettungscode");
define("CMS_L_IFC_MEDIA_015", "Wert einfügen …");
define("CMS_L_IFC_MEDIA_016", "Die Quelle ist extern und kann nicht intern ersetzt werden. " .
                              "Wählen Sie <em>Bearbeiten</em>, um die Quelle zu ändern.");
define("CMS_L_IFC_MEDIA_017", "Erweitert");
define("CMS_L_IFC_MEDIA_018", "Dateiname");
define("CMS_L_IFC_MEDIA_019", "Kategorie");
define("CMS_L_IFC_MEDIA_020", "Bulk-Upload");
define("CMS_L_IFC_MEDIA_021", "Hier können Sie mehrere Dateien zum Upload auswählen.");
define("CMS_L_IFC_MEDIA_025", "Daten");

//ifc_permission
//..................................................................................................
define("CMS_L_IFC_PERMISSION",     "Administration");
define("CMS_L_IFC_PERMISSION_001", "Konto erstellen");
define("CMS_L_IFC_PERMISSION_002", "Benutzername");
define("CMS_L_IFC_PERMISSION_003", "Passwortwiederholung");
define("CMS_L_IFC_PERMISSION_004", "Gruppe erstellen");
define("CMS_L_IFC_PERMISSION_005", "Gruppen-ID");
define("CMS_L_IFC_PERMISSION_006", "Anmerkung");
define("CMS_L_IFC_PERMISSION_007", "Gruppenzugehörigkeit");
define("CMS_L_IFC_PERMISSION_008", "Gesperrt");
define("CMS_L_IFC_PERMISSION_009", "Erteilter Zugriff");
define("CMS_L_IFC_PERMISSION_010", "Konto");
define("CMS_L_IFC_PERMISSION_011", "Ablauf (Tage)");
define("CMS_L_IFC_PERMISSION_012", "Mitglieder");
define("CMS_L_IFC_PERMISSION_013", "Verweigerter Zugriff hat Vorrang.");
define("CMS_L_IFC_PERMISSION_014", "Rechteverwaltung");
define("CMS_L_IFC_PERMISSION_015", "Zugriff erteilt");
define("CMS_L_IFC_PERMISSION_016", "Zugriff verweigert");
define("CMS_L_IFC_PERMISSION_017", "Zeitzone");
define("CMS_L_IFC_PERMISSION_018", "Verweigerter Zugriff");
define("CMS_L_IFC_PERMISSION_019", "Berechtigungen");
define("CMS_L_IFC_PERMISSION_020", "Erweiterte Einstellungen");
define("CMS_L_IFC_PERMISSION_021", "Benutzer");
define("CMS_L_IFC_PERMISSION_022", "Gruppen");
define("CMS_L_IFC_PERMISSION_026", "Markierte entsperren");
define("CMS_L_IFC_PERMISSION_027", "Markierte sperren");
define("CMS_L_IFC_PERMISSION_028", "Explizite Berechtigung entfernen");
define("CMS_L_IFC_PERMISSION_029", "Expliziten Ausschluss entfernen");
define("CMS_L_IFC_PERMISSION_030", "<strong>⁺</strong> explizit erteilt");
define("CMS_L_IFC_PERMISSION_031", "<strong>⁺</strong> explizit erteilt, <strong>⁻</strong> explizit verweigert");
define("CMS_L_IFC_PERMISSION_032", "Zugangsschlüssel");
define("CMS_L_IFC_PERMISSION_033", "Passwort und Wiederholung stimmen nicht überein. " .
                                   "Das Passwort wurde nicht geändert.");

//ifc_profile
//..................................................................................................
define("CMS_L_IFC_PROFILE",     "Profile");
define("CMS_L_IFC_PROFILE_001", "ID");
define("CMS_L_IFC_PROFILE_002", "Kennung");
define("CMS_L_IFC_PROFILE_003", "Benutzername");
define("CMS_L_IFC_PROFILE_004", "Rechtevererbung");
define("CMS_L_IFC_PROFILE_005", "Organisation");
define("CMS_L_IFC_PROFILE_006", "Vorname");
define("CMS_L_IFC_PROFILE_007", "Nachname");
define("CMS_L_IFC_PROFILE_008", "Straße");
define("CMS_L_IFC_PROFILE_009", "PLZ");
define("CMS_L_IFC_PROFILE_010", "Ort");
define("CMS_L_IFC_PROFILE_011", "Land");
define("CMS_L_IFC_PROFILE_012", "Telefon");
define("CMS_L_IFC_PROFILE_013", "Telefon 2");
define("CMS_L_IFC_PROFILE_014", "Mobiltelefon");
define("CMS_L_IFC_PROFILE_015", "Fax");
define("CMS_L_IFC_PROFILE_016", "Anmerkung");
define("CMS_L_IFC_PROFILE_017", "Beginnt mit");
define("CMS_L_IFC_PROFILE_018", "Enthält");
define("CMS_L_IFC_PROFILE_019", "Endet mit");
define("CMS_L_IFC_PROFILE_020", "Größer als");
define("CMS_L_IFC_PROFILE_021", "Gleich");
define("CMS_L_IFC_PROFILE_022", "Kleiner als");
define("CMS_L_IFC_PROFILE_023", "Gruppierung");
define("CMS_L_IFC_PROFILE_024", "Zeilen");
define("CMS_L_IFC_PROFILE_025", "Reset");
define("CMS_L_IFC_PROFILE_026", "(Leer)");
define("CMS_L_IFC_PROFILE_027", "Verwaltungsdaten");
define("CMS_L_IFC_PROFILE_028", "Personendaten");
define("CMS_L_IFC_PROFILE_029", "Kontaktdaten");
define("CMS_L_IFC_PROFILE_030", "Sonstige Daten");
define("CMS_L_IFC_PROFILE_031", "(Keine Angabe)");
define("CMS_L_IFC_PROFILE_032", "Sichtbar");
define("CMS_L_IFC_PROFILE_033", "Änderbar");
define("CMS_L_IFC_PROFILE_034", "Basiseinstellungen");
define("CMS_L_IFC_PROFILE_035", "Erforderlich");
define("CMS_L_IFC_PROFILE_036", "Erweiterte Einstellungen");
define("CMS_L_IFC_PROFILE_037", "Erweiterte Informationen");
define("CMS_L_IFC_PROFILE_038", "Zusatzfeld");
define("CMS_L_IFC_PROFILE_039", "Filter");
define("CMS_L_IFC_PROFILE_040", "Seite");
define("CMS_L_IFC_PROFILE_041", "Aktiv");
define("CMS_L_IFC_PROFILE_042", "Kreditkartendaten");
define("CMS_L_IFC_PROFILE_043", "Bankverbindung");
define("CMS_L_IFC_PROFILE_044", "Kreditkartennummer");
define("CMS_L_IFC_PROFILE_045", "Kreditinstitut");
define("CMS_L_IFC_PROFILE_046", "Kreditkarteninhaber");
define("CMS_L_IFC_PROFILE_047", "Ablaufdatum");
define("CMS_L_IFC_PROFILE_048", "Kontonummer");
define("CMS_L_IFC_PROFILE_049", "Geldinstitut");
define("CMS_L_IFC_PROFILE_050", "Bankleitzahl");
define("CMS_L_IFC_PROFILE_051", "Kontoinhaber");
define("CMS_L_IFC_PROFILE_053", "Erstellt");
define("CMS_L_IFC_PROFILE_054", "Geändert");
define("CMS_L_IFC_PROFILE_055", "Generieren");

//ifc_search
//..................................................................................................
define("CMS_L_IFC_SEARCH",     "Suche");
define("CMS_L_IFC_SEARCH_001", "Filter");
define("CMS_L_IFC_SEARCH_002", "Titel");
define("CMS_L_IFC_SEARCH_003", "Queue: <strong>%u Aufträge gesamt</strong> " .
                               "<a href=\"%s\" class=\"green\">[Abarbeiten]</a> " .
                               "<a href=\"%s\" class=\"red\">[Löschen]</a>, " .
                               "<strong>%u interne Seiten</strong> " .
                               "<a href=\"%s\" class=\"green\">[Abarbeiten]</a> " .
                               "<a href=\"%s\" class=\"red\">[Löschen]</a>, " .
                               "<strong>%u Backend-Aufträge</strong> " .
                               "<a href=\"%s\" class=\"green\">[Abarbeiten]</a> " .
                               "<a href=\"%s\" class=\"red\">[Löschen]</a>, " .
                               "<strong>%u Frontend-Aufträge</strong> " .
                               "<a href=\"%s\" class=\"green\">[Abarbeiten]</a> " .
                               "<a href=\"%s\" class=\"red\">[Löschen]</a>, " .
                               "<strong>%u Links</strong> " .
                               "<a href=\"%s\" class=\"green\">[Abarbeiten]</a> " .
                               "<a href=\"%s\" class=\"red\">[Löschen]</a>, " .
                               "<strong>%u Updates</strong> " .
                               "<a href=\"%s\" class=\"green\">[Abarbeiten]</a> " .
                               "<a href=\"%s\" class=\"red\">[Löschen]</a>.");
define("CMS_L_IFC_SEARCH_004", "Inhalt");
define("CMS_L_IFC_SEARCH_005", "Stand");
define("CMS_L_IFC_SEARCH_006", "Beginnt mit");
define("CMS_L_IFC_SEARCH_007", "Enthält");
define("CMS_L_IFC_SEARCH_008", "Endet mit");
define("CMS_L_IFC_SEARCH_009", "Größer als");
define("CMS_L_IFC_SEARCH_010", "Gleich");
define("CMS_L_IFC_SEARCH_011", "Kleiner als");
define("CMS_L_IFC_SEARCH_012", "Zeilen");
define("CMS_L_IFC_SEARCH_013", "Redirect-Limit erreicht");
define("CMS_L_IFC_SEARCH_014", "Abrufen");
define("CMS_L_IFC_SEARCH_015", "Alles updaten");
define("CMS_L_IFC_SEARCH_016", "Links erfassen");
define("CMS_L_IFC_SEARCH_017", "Scores berechnen");
define("CMS_L_IFC_SEARCH_018", "Seite eintragen");
define("CMS_L_IFC_SEARCH_019", "Alles abarbeiten");
define("CMS_L_IFC_SEARCH_020", "Interne Seiten abarbeiten");
define("CMS_L_IFC_SEARCH_021", "Frontend-Aufträge abarbeiten");
define("CMS_L_IFC_SEARCH_022", "Links abarbeiten");
define("CMS_L_IFC_SEARCH_023", "Updates abarbeiten");
define("CMS_L_IFC_SEARCH_024", "Update");
define("CMS_L_IFC_SEARCH_025", "Datenbank optimieren");
define("CMS_L_IFC_SEARCH_026", "Interne Seiten erfassen");
define("CMS_L_IFC_SEARCH_027", "Backend-Einträge abarbeiten");
define("CMS_L_IFC_SEARCH_028", "Markierte updaten");
define("CMS_L_IFC_SEARCH_029", "Lade …");
define("CMS_L_IFC_SEARCH_030", "Fehler");
define("CMS_L_IFC_SEARCH_031", "OK");
define("CMS_L_IFC_SEARCH_032", "Fertig");
define("CMS_L_IFC_SEARCH_033", "Ungültige URL");
define("CMS_L_IFC_SEARCH_034", "Keine Verbindung");
define("CMS_L_IFC_SEARCH_035", "Format nicht unterstützt");
define("CMS_L_IFC_SEARCH_036", "Keine Änderung");
define("CMS_L_IFC_SEARCH_037", "%s verbleibend");
define("CMS_L_IFC_SEARCH_038", "Datenabruf fehlgeschlagen");
define("CMS_L_IFC_SEARCH_039", "Kein Inhalt");
define("CMS_L_IFC_SEARCH_040", "Indizierung untersagt");
define("CMS_L_IFC_SEARCH_041", "Indiziert");
define("CMS_L_IFC_SEARCH_042", "Indizierung fehlgeschlagen");
define("CMS_L_IFC_SEARCH_043", "Unbekannter Fehler");
define("CMS_L_IFC_SEARCH_044", "Reset");
define("CMS_L_IFC_SEARCH_045", "Sortierung");
define("CMS_L_IFC_SEARCH_046", "Presse Linksaft …");
define("CMS_L_IFC_SEARCH_047", "Initialisiere Scores");
define("CMS_L_IFC_SEARCH_048", "Berechne Scores (%u/%u)");
define("CMS_L_IFC_SEARCH_049", "Finalisiere Scores");
define("CMS_L_IFC_SEARCH_050", "Dangling Links einbinden (%u/%u)");
define("CMS_L_IFC_SEARCH_051", "URL abgelehnt");
define("CMS_L_IFC_SEARCH_052", "URL-Filter");
define("CMS_L_IFC_SEARCH_053", "Ausschlussregeln");
define("CMS_L_IFC_SEARCH_054", "Ausnahmeregeln");
define("CMS_L_IFC_SEARCH_055", "Verwenden Sie reguläre Ausdrücke (" .
                               "<a href=\"https://www.pcre.org/pcre.txt\" " .
                                  "onclick=\"window.open(this.href);return false;\" " .
                                  "rel=\"noopener noreferrer\">" .
                               "PCRE</a>), " .
                               "um URLs zu sperren, indem Sie einen Ausdruck pro Zeile angeben. " .
                               "Die Ausnahmeregeln legen Ausnahmen dazu fest. " .
                               "Ein Ausdruck lässt sich durch ein \"!\" am Zeilenanfang umkehren.<br>" .
                               "<br>" .
                               "Beispiel: !~^https://(?:[^\.]+\.)?beispiel\.de(?:/\|$)~i<br>" .
                               "Alle URLs, die nicht zur Domain beispiel.de gehören.");
define("CMS_L_IFC_SEARCH_056", "Finde kanonische Seiten");
define("CMS_L_IFC_SEARCH_057", "Ausführen");
define("CMS_L_IFC_SEARCH_058", "Score");
define("CMS_L_IFC_SEARCH_059", "Stoppwörter");
define("CMS_L_IFC_SEARCH_060", "Stoppwörter (Whitespace-getrennt)");
define("CMS_L_IFC_SEARCH_061", "Legen Sie Wörter fest, " .
                               "die in der jeweiligen Sprache keine inhaltliche Relevanz haben, " .
                               "z.B. Artikel, Konjunktionen, Präpositionen, etc. " .
                               "Das Filtern dieser Wörter ermöglicht eine effizientere Suche. " .
                               "Trennen Sie jedes Wort durch einen Whitespace " .
                               "(Leerzeichen, Zeilenumbruch, etc.)");
define("CMS_L_IFC_SEARCH_062", "Einstellungen");
define("CMS_L_IFC_SEARCH_063", "Max. Abweichung für Ähnlichkeit");
define("CMS_L_IFC_SEARCH_064", "(Default)");
define("CMS_L_IFC_SEARCH_065", "Daemon");
define("CMS_L_IFC_SEARCH_066", "Aktivieren");
define("CMS_L_IFC_SEARCH_067", "Suche");

//ifc_setup
//..................................................................................................
define("CMS_L_IFC_SETUP",     "System");
define("CMS_L_IFC_SETUP_001", "Administrator-Passwort");
define("CMS_L_IFC_SETUP_002", "Das Administrator-Passwort entspricht noch der Vorgabe und ist nicht sicher. " .
                              "Bitte geben Sie ein neues Passwort ein, " .
                              "mit dem Sie sich als Administrator anmelden.");
define("CMS_L_IFC_SETUP_003", "Neues Passwort");
define("CMS_L_IFC_SETUP_004", "Passwortwiederholung");
define("CMS_L_IFC_SETUP_005", "Datenbank-Verbindung");
define("CMS_L_IFC_SETUP_006", "Bitte geben Sie die Zugangsdaten für die MySQL-Datenbank an, " .
                              "die Sie zum Speichern ihrer Anwendungsdaten verwenden möchten.");
define("CMS_L_IFC_SETUP_007", "MySQL-Server");
define("CMS_L_IFC_SETUP_008", "MySQL-Datenbank");
define("CMS_L_IFC_SETUP_009", "Benutzername");
define("CMS_L_IFC_SETUP_010", "Passwort");
define("CMS_L_IFC_SETUP_011", "Sicherung herunterladen");
define("CMS_L_IFC_SETUP_012", "Postausgang-Server");
define("CMS_L_IFC_SETUP_013", "Bitte geben Sie die E-Mail-Adresse an, " .
                              "die für den Versand von Formulardaten verwendet werden soll " .
                              "und wählen Sie die Versandmethode.");
define("CMS_L_IFC_SETUP_014", "SMTP-Server");
define("CMS_L_IFC_SETUP_015", "E-Mail (From)");
define("CMS_L_IFC_SETUP_016", "Benutzername");
define("CMS_L_IFC_SETUP_017", "Passwort");
define("CMS_L_IFC_SETUP_018", "Nein, Schritt überspringen.");
define("CMS_L_IFC_SETUP_019", "Ja, Daten schreiben.");
define("CMS_L_IFC_SETUP_020", "Sie haben kein Passwort angegeben.");
define("CMS_L_IFC_SETUP_021", "Passwort und Wiederholung stimmen nicht überein.");
define("CMS_L_IFC_SETUP_022", "Es konnte keine Verbindung mit der Datenbank hergestellt werden.");
define("CMS_L_IFC_SETUP_023", "Es konnte keine Verbindung mit dem Postausgang-Server hergestellt werden.");
define("CMS_L_IFC_SETUP_024", "Das Setup nach der Anmeldung nicht mehr anzeigen.");
define("CMS_L_IFC_SETUP_025", "mail()-Funktion verwenden");
define("CMS_L_IFC_SETUP_026", "SMTP-Server verwenden");
define("CMS_L_IFC_SETUP_027", "SMTP-Einstellungen");
define("CMS_L_IFC_SETUP_028", "[TEST] Automatisch generierte Testnachricht");
define("CMS_L_IFC_SETUP_029", "Datenbank-Initialisierung");
define("CMS_L_IFC_SETUP_030", "Möchten Sie die Initialdaten in die Datenbank schreiben (empfohlen)? " .
                              "Bereits existierende Tabellen werden dabei nicht überschrieben, " .
                              "sondern als Backup gesichert.");
define("CMS_L_IFC_SETUP_031", "Die Initialdaten konnten nicht in die Datenbank geschrieben werden.");
define("CMS_L_IFC_SETUP_032", "Warte auf Ausführung …");
define("CMS_L_IFC_SETUP_033", "Die Updateversion konnte nicht ermittelt werden.");
define("CMS_L_IFC_SETUP_034", "Erneut versuchen");
define("CMS_L_IFC_SETUP_035", "Das System ist auf dem neuesten Stand.");
define("CMS_L_IFC_SETUP_036", "Eine neuere Version ist verfügbar: <strong>%s</strong><br>" .
                              "Derzeitige Version: %s");
define("CMS_L_IFC_SETUP_037", "Update jetzt installieren");
define("CMS_L_IFC_SETUP_038", "Update-Installation");
define("CMS_L_IFC_SETUP_039", "Keine Antwort vom Server erhalten.");
define("CMS_L_IFC_SETUP_040", "Sicherung überspringen (nicht empfohlen)");
define("CMS_L_IFC_SETUP_041", "Dies ist eine automatisch generierte Testnachricht, " .
                              "um die Funktion des E-Mail-Versands zu prüfen. " .
                              "Keine Aktion Ihrerseits ist erforderlich. " .
                              "Sie können diese Nachricht bedenkenlos löschen. " .
                              "Der Erhalt dieser Mail zeigt, " .
                              "dass der Versand korrekt ausgeführt wird.");
define("CMS_L_IFC_SETUP_042", "Sicherung");
define("CMS_L_IFC_SETUP_043", "Neue Sicherung erstellen");
define("CMS_L_IFC_SETUP_044", "Sicherungsvorgang");
define("CMS_L_IFC_SETUP_045", "Daemon-Monitor");
define("CMS_L_IFC_SETUP_046", "Unbekannter Fehler");
define("CMS_L_IFC_SETUP_047", "E-Mail (Reply-To)");

//ifc_system
//..................................................................................................
define("CMS_L_IFC_SYSTEM",     "#System");
define("CMS_L_IFC_SYSTEM_001", "Attribut");
define("CMS_L_IFC_SYSTEM_002", "Untergeordnete Elemente zulassen");
define("CMS_L_IFC_SYSTEM_006", "Schritt 1: Einstellungen");
define("CMS_L_IFC_SYSTEM_007", "Wert");
define("CMS_L_IFC_SYSTEM_008", "Schritt 2: Position");
define("CMS_L_IFC_SYSTEM_009", "Index");

//ifc_template
//..................................................................................................
define("CMS_L_IFC_TEMPLATE",     "Templates");
define("CMS_L_IFC_TEMPLATE_001", "Quellcode von URL");
define("CMS_L_IFC_TEMPLATE_002", "Leerer Quellcode");
define("CMS_L_IFC_TEMPLATE_003", "Quellcode aus Datei");
define("CMS_L_IFC_TEMPLATE_004", "Stylesheet aus Template einfügen");
define("CMS_L_IFC_TEMPLATE_005", "Kategorie");
define("CMS_L_IFC_TEMPLATE_006", "Quellcode");
define("CMS_L_IFC_TEMPLATE_007", "Template einfügen");
define("CMS_L_IFC_TEMPLATE_008", "Seitentemplate");
define("CMS_L_IFC_TEMPLATE_009", "Vorschau nicht verfügbar. " .
                                 "IFRAMEs sind deaktiviert oder werden von ihrer Websoftware nicht unterstützt.");
define("CMS_L_IFC_TEMPLATE_010", "Bild einfügen");
define("CMS_L_IFC_TEMPLATE_011", "Einfügen …");
define("CMS_L_IFC_TEMPLATE_012", "Quellcode beibehalten");
define("CMS_L_IFC_TEMPLATE_013", "Platzhalter einfügen");
define("CMS_L_IFC_TEMPLATE_014", "Headerdaten");
define("CMS_L_IFC_TEMPLATE_015", "Kontrollpad");
define("CMS_L_IFC_TEMPLATE_016", "Abhängiger Bereich");
define("CMS_L_IFC_TEMPLATE_017", "Abhängige Alternative");
define("CMS_L_IFC_TEMPLATE_018", "Wiederholung");
define("CMS_L_IFC_TEMPLATE_019", "Template");
define("CMS_L_IFC_TEMPLATE_020", "Text");
define("CMS_L_IFC_TEMPLATE_021", "Link");
define("CMS_L_IFC_TEMPLATE_022", "Bild");
define("CMS_L_IFC_TEMPLATE_023", "Thumbnail");
define("CMS_L_IFC_TEMPLATE_024", "Media");
define("CMS_L_IFC_TEMPLATE_025", "Download");
define("CMS_L_IFC_TEMPLATE_026", "Plugin");
define("CMS_L_IFC_TEMPLATE_027", "Script");
define("CMS_L_IFC_TEMPLATE_028", "Nur Bearbeitung");
define("CMS_L_IFC_TEMPLATE_029", "Kommentar");
define("CMS_L_IFC_TEMPLATE_030", "Seitengerüst");
define("CMS_L_IFC_TEMPLATE_031", "Backlink");
define("CMS_L_IFC_TEMPLATE_032", "HTML");
define("CMS_L_IFC_TEMPLATE_033", "PHP-Include");
define("CMS_L_IFC_TEMPLATE_034", "Kategorie umbenennen");
define("CMS_L_IFC_TEMPLATE_035", "Positionsänderung");
define("CMS_L_IFC_TEMPLATE_036", "Kein Caching");
define("CMS_L_IFC_TEMPLATE_037", "Gruppe");
define("CMS_L_IFC_TEMPLATE_038", "Namensraum");
define("CMS_L_IFC_TEMPLATE_039", "Basis-Namensraum");
define("CMS_L_IFC_TEMPLATE_040", "Wert");
define("CMS_L_IFC_TEMPLATE_041", "CSS");
define("CMS_L_IFC_TEMPLATE_042", "Link einfügen");
define("CMS_L_IFC_TEMPLATE_043", "PHP-Ausgabe");
define("CMS_L_IFC_TEMPLATE_044", "Speichern und anzeigen");
define("CMS_L_IFC_TEMPLATE_045", "Navigation");
define("CMS_L_IFC_TEMPLATE_046", "Download einfügen");
define("CMS_L_IFC_TEMPLATE_047", "Link einfügen (PHP-Code)");
define("CMS_L_IFC_TEMPLATE_048", "Bild einfügen (PHP-Code)");
define("CMS_L_IFC_TEMPLATE_049", "Media einfügen");
define("CMS_L_IFC_TEMPLATE_050", "PHP");
define("CMS_L_IFC_TEMPLATE_051", "CMS-Elemente");
define("CMS_L_IFC_TEMPLATE_052", "Schalter");
define("CMS_L_IFC_TEMPLATE_053", "Nur Ausgabe");
define("CMS_L_IFC_TEMPLATE_054", "Stylesheet");
define("CMS_L_IFC_TEMPLATE_055", "JavaScript");
define("CMS_L_IFC_TEMPLATE_056", "JavaScript aus Template einfügen");

//ifc_token
//..................................................................................................
define("CMS_L_IFC_TOKEN",     "Platzhalter");
define("CMS_L_IFC_TOKEN_001", "Kategorie");
define("CMS_L_IFC_TOKEN_002", "Platzhalter"); //plural
define("CMS_L_IFC_TOKEN_003", "Platzhalter"); //singular
define("CMS_L_IFC_TOKEN_004", "Vorschau nicht verfügbar. " .
                              "IFRAMEs sind deaktiviert oder werden von ihrer Websoftware nicht unterstützt.");
define("CMS_L_IFC_TOKEN_005", "Parameter einfügen …");
define("CMS_L_IFC_TOKEN_006", "Ersetzungscode");
define("CMS_L_IFC_TOKEN_007", "Code");
define("CMS_L_IFC_TOKEN_008", "Parameter (kommagetrennt)");
define("CMS_L_IFC_TOKEN_009", "Kategorie umbenennen");

//==================================================================================================
//   DESKTOP
//==================================================================================================

//desktop_address
//..................................................................................................
define("CMS_L_DESKTOP_ADDRESS_001", "Kontakt");
define("CMS_L_DESKTOP_ADDRESS_002", "Notiz");
define("CMS_L_DESKTOP_ADDRESS_003", "Organisation");
define("CMS_L_DESKTOP_ADDRESS_004", "Telefon");
define("CMS_L_DESKTOP_ADDRESS_005", "Fax");
define("CMS_L_DESKTOP_ADDRESS_006", "Kontakt");

//desktop_appointment
//..................................................................................................
define("CMS_L_DESKTOP_APPOINTMENT_001", "Neuer Termin");
define("CMS_L_DESKTOP_APPOINTMENT_002", "Termin");
define("CMS_L_DESKTOP_APPOINTMENT_003", "Datum");
define("CMS_L_DESKTOP_APPOINTMENT_004", "Zeit");
define("CMS_L_DESKTOP_APPOINTMENT_005", "Betreff");
define("CMS_L_DESKTOP_APPOINTMENT_006", "Ort");
define("CMS_L_DESKTOP_APPOINTMENT_007", "Datum / Zeit");
define("CMS_L_DESKTOP_APPOINTMENT_011", "Nach Ablauf löschen");
define("CMS_L_DESKTOP_APPOINTMENT_012", "Inhalt");
define("CMS_L_DESKTOP_APPOINTMENT_013", "Teilnehmer");

//desktop_ims
//..................................................................................................
define("CMS_L_DESKTOP_IMS_001", "Nachricht schreiben");
define("CMS_L_DESKTOP_IMS_002", "Empfänger");
define("CMS_L_DESKTOP_IMS_003", "Nachricht");
define("CMS_L_DESKTOP_IMS_004", "Keine Empfänger vorhanden.");
define("CMS_L_DESKTOP_IMS_005", "Keine Nachrichten vorhanden.");
define("CMS_L_DESKTOP_IMS_009", "Benutzerkorrespondenz anzeigen");
define("CMS_L_DESKTOP_IMS_010", "Zusammenhängende Nachrichten anzeigen");
define("CMS_L_DESKTOP_IMS_011", "Antworten");
define("CMS_L_DESKTOP_IMS_012", "Messenger");
define("CMS_L_DESKTOP_IMS_013", "an");
define("CMS_L_DESKTOP_IMS_014", "von");

//desktop_link
//..................................................................................................
define("CMS_L_DESKTOP_LINK_001", "Menü entfernen");
define("CMS_L_DESKTOP_LINK_002", "Link");
define("CMS_L_DESKTOP_LINK_003", "Neu laden");

//desktop_mailbox
//..................................................................................................
define("CMS_L_DESKTOP_MAILBOX_001", "Eingang");
define("CMS_L_DESKTOP_MAILBOX_002", "Ausgang");
define("CMS_L_DESKTOP_MAILBOX_003", "Entwürfe");
define("CMS_L_DESKTOP_MAILBOX_004", "Papierkorb");
define("CMS_L_DESKTOP_MAILBOX_005", "Spam");
define("CMS_L_DESKTOP_MAILBOX_006", "%u neue Nachricht(en), davon %u als Spam erkannt.");
define("CMS_L_DESKTOP_MAILBOX_007", "Nachricht");
define("CMS_L_DESKTOP_MAILBOX_008", "Absender");
define("CMS_L_DESKTOP_MAILBOX_009", "AW");
define("CMS_L_DESKTOP_MAILBOX_010", "Sendedatum");
define("CMS_L_DESKTOP_MAILBOX_011", "Empfänger");
define("CMS_L_DESKTOP_MAILBOX_012", "Nebenempfänger");
define("CMS_L_DESKTOP_MAILBOX_013", "<small>Format:</small> " .
                                    "<strong>%s</strong>, " .
                                    "<small>Größe:</small> %s");
define("CMS_L_DESKTOP_MAILBOX_014", "Nachricht schreiben");
define("CMS_L_DESKTOP_MAILBOX_015", "CC (Kopie)");
define("CMS_L_DESKTOP_MAILBOX_016", "BCC (Blindkopie)");
define("CMS_L_DESKTOP_MAILBOX_017", "Betreff");
define("CMS_L_DESKTOP_MAILBOX_018", "Kontoname");
define("CMS_L_DESKTOP_MAILBOX_019", "Posteingangsserver (POP3)");
define("CMS_L_DESKTOP_MAILBOX_020", "Postausgangsserver (SMTP)");
define("CMS_L_DESKTOP_MAILBOX_021", "Signatur");
define("CMS_L_DESKTOP_MAILBOX_022", "Spamfilter-Schwellenwert");
define("CMS_L_DESKTOP_MAILBOX_023", "Deaktiviert");
define("CMS_L_DESKTOP_MAILBOX_024", "Empfangen");
define("CMS_L_DESKTOP_MAILBOX_025", "Senden");
define("CMS_L_DESKTOP_MAILBOX_026", "Ordner erstellen");
define("CMS_L_DESKTOP_MAILBOX_027", "Ordner umbenennen");
define("CMS_L_DESKTOP_MAILBOX_028", "Ordner löschen");
define("CMS_L_DESKTOP_MAILBOX_029", "Einstellungen");
define("CMS_L_DESKTOP_MAILBOX_030", "Nachricht lesen");
define("CMS_L_DESKTOP_MAILBOX_031", "Postfach");
define("CMS_L_DESKTOP_MAILBOX_032", "Ordner");
define("CMS_L_DESKTOP_MAILBOX_036", "Verschieben nach");
define("CMS_L_DESKTOP_MAILBOX_037", "Datum");
define("CMS_L_DESKTOP_MAILBOX_038", "Nach Datum sortieren");
define("CMS_L_DESKTOP_MAILBOX_039", "Absender");
define("CMS_L_DESKTOP_MAILBOX_040", "Nach Absender sortieren");
define("CMS_L_DESKTOP_MAILBOX_041", "Spam");
define("CMS_L_DESKTOP_MAILBOX_042", "Nach Spam-Wahrscheinlichkeit sortieren");
define("CMS_L_DESKTOP_MAILBOX_043", "Größe");
define("CMS_L_DESKTOP_MAILBOX_044", "Nach Größe sortieren");
define("CMS_L_DESKTOP_MAILBOX_045", "Betreff");
define("CMS_L_DESKTOP_MAILBOX_046", "Nach Betreff sortieren");
define("CMS_L_DESKTOP_MAILBOX_048", "Als Spam trainieren");
define("CMS_L_DESKTOP_MAILBOX_049", "Als Normal trainieren");
define("CMS_L_DESKTOP_MAILBOX_050", "Papierkorb leeren");
define("CMS_L_DESKTOP_MAILBOX_051", "Nachrichten nach Empfang vom Server löschen");
define("CMS_L_DESKTOP_MAILBOX_053", "Dateianhang");
define("CMS_L_DESKTOP_MAILBOX_054", "-------- Ursprüngliche Nachricht --------");
define("CMS_L_DESKTOP_MAILBOX_055", "Die weiterzuleitende Nachricht wurde angehängt.");
define("CMS_L_DESKTOP_MAILBOX_056", "Antworten");
define("CMS_L_DESKTOP_MAILBOX_057", "Weiterleiten");
define("CMS_L_DESKTOP_MAILBOX_058", "AW");
define("CMS_L_DESKTOP_MAILBOX_059", "WG");
define("CMS_L_DESKTOP_MAILBOX_060", "%u gelöschte Nachricht(en), davon %u vom Postserver gelöscht.");
define("CMS_L_DESKTOP_MAILBOX_061", "Enthält Anhang");

//desktop_note
//..................................................................................................
define("CMS_L_DESKTOP_NOTE_001", "Notiz");