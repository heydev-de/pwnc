CREATE TABLE `cms_forum_de` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `container` int unsigned NOT NULL DEFAULT '0',
  `user` varchar(40) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
  `title` varchar(80) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
  `text` text COLLATE utf8mb4_unicode_ci NOT NULL,
  `email` tinyint(1) NOT NULL DEFAULT '0',
  `time` int unsigned NOT NULL DEFAULT '0',
  `access` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  FULLTEXT KEY `title` (`title`),
  FULLTEXT KEY `text` (`text`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci