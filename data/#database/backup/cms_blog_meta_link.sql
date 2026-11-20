CREATE TABLE `cms_blog_meta_link` (
  `article` int unsigned NOT NULL DEFAULT '0',
  `term` int unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `article` (`article`,`term`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci