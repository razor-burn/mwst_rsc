-- MySQL dump 10.13  Distrib 5.5.14, for Win32 (x86)
--
-- Host: localhost    Database: mwst_rsc
-- ------------------------------------------------------
-- Server version	5.5.14

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `adminuser`
--

DROP TABLE IF EXISTS `adminuser`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `adminuser` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(80) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `allloanpositions_view`
--

DROP TABLE IF EXISTS `allloanpositions_view`;
/*!50001 DROP VIEW IF EXISTS `allloanpositions_view`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `allloanpositions_view` (
  `posid` int(10) unsigned,
  `loanid` int(10) unsigned,
  `Menge` int(10) unsigned,
  `Retourmenge` int(10) unsigned,
  `Hersteller` varchar(255),
  `Typ` varchar(255),
  `id` int(10) unsigned
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `allloans_view`
--

DROP TABLE IF EXISTS `allloans_view`;
/*!50001 DROP VIEW IF EXISTS `allloans_view`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `allloans_view` (
  `loanid` int(11) unsigned,
  `userid` int(10) unsigned,
  `Name` varchar(80),
  `Vorname` varchar(80),
  `Beginn` datetime,
  `Ende` datetime,
  `Mitarbeiter` varchar(80),
  `isactive` tinyint(1) unsigned
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `allresources_view`
--

DROP TABLE IF EXISTS `allresources_view`;
/*!50001 DROP VIEW IF EXISTS `allresources_view`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `allresources_view` (
  `Kategorie` varchar(45),
  `KategorieID` int(11) unsigned,
  `Raum` varchar(50),
  `Schrank` varchar(45),
  `Hersteller` varchar(255),
  `Typ` varchar(255),
  `Beschreibung` varchar(500),
  `ResourceID` int(10) unsigned,
  `Menge` int(10) unsigned,
  `MinMenge` int(10) unsigned
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `allroomloans_view`
--

DROP TABLE IF EXISTS `allroomloans_view`;
/*!50001 DROP VIEW IF EXISTS `allroomloans_view`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `allroomloans_view` (
  `EditingroomloanID` int(10) unsigned,
  `RoomID` int(10) unsigned,
  `Raum` varchar(50),
  `Start` datetime,
  `Ende` datetime,
  `Name` varchar(80),
  `Vorname` varchar(80),
  `Email` varchar(100),
  `Telefon` varchar(45),
  `Mitarbeiter` varchar(80)
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `categories`
--

DROP TABLE IF EXISTS `categories`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `categories` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
ALTER TABLE categories AUTO_INCREMENT = 0;
INSERT INTO categories (id, name) VALUES (0, 'Nicht zugeordnet');

--
-- Table structure for table `editingroomloans`
--

DROP TABLE IF EXISTS `editingroomloans`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `editingroomloans` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `room` int(10) unsigned NOT NULL,
  `user` int(10) unsigned NOT NULL DEFAULT '0',
  `start` datetime NOT NULL,
  `end` datetime NOT NULL,
  `adminuser` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `roomid` (`room`),
  KEY `admin` (`adminuser`),
  KEY `user2` (`user`),
  CONSTRAINT `admin` FOREIGN KEY (`adminuser`) REFERENCES `adminuser` (`id`),
  CONSTRAINT `roomid` FOREIGN KEY (`room`) REFERENCES `editingrooms` (`id`),
  CONSTRAINT `user2` FOREIGN KEY (`user`) REFERENCES `users` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `editingrooms`
--

DROP TABLE IF EXISTS `editingrooms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `editingrooms` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL DEFAULT '''''',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `loanposition`
--

DROP TABLE IF EXISTS `loanposition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `loanposition` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `loanid` int(10) unsigned NOT NULL DEFAULT '0',
  `resourceid` int(10) unsigned NOT NULL DEFAULT '0',
  `quantity` int(10) unsigned NOT NULL DEFAULT '0',
  `returnedquantity` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `resourceid` (`resourceid`),
  KEY `loanid` (`loanid`),
  CONSTRAINT `loanid` FOREIGN KEY (`loanid`) REFERENCES `loans` (`id`),
  CONSTRAINT `resourceid` FOREIGN KEY (`resourceid`) REFERENCES `resources` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=16 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `loans`
--

DROP TABLE IF EXISTS `loans`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `loans` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `user` int(10) unsigned NOT NULL,
  `start` datetime NOT NULL,
  `end` datetime NOT NULL,
  `created` datetime NOT NULL,
  `adminuser` int(10) unsigned NOT NULL,
  `goesabroad` tinyint(1) unsigned NOT NULL,
  `isactive` tinyint(1) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `user` (`user`),
  KEY `admin2` (`adminuser`),
  CONSTRAINT `admin2` FOREIGN KEY (`adminuser`) REFERENCES `adminuser` (`id`),
  CONSTRAINT `user` FOREIGN KEY (`user`) REFERENCES `users` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `resources`
--

DROP TABLE IF EXISTS `resources`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `resources` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `equipmentnr` int(10) unsigned DEFAULT '0',
  `activedate` date DEFAULT NULL,
  `costlocation` int(10) unsigned DEFAULT '0',
  `room` int(10) unsigned NOT NULL DEFAULT '0',
  `locker` varchar(45) DEFAULT NULL,
  `make` varchar(255) NOT NULL DEFAULT '',
  `type` varchar(255) NOT NULL DEFAULT '',
  `description` varchar(500) NOT NULL DEFAULT '',
  `serialnr` varchar(45) DEFAULT NULL,
  `purchaseprice` double unsigned NOT NULL DEFAULT '1',
  `totalquantity` int(10) unsigned NOT NULL DEFAULT '0',
  `quantity` int(10) unsigned NOT NULL DEFAULT '0',
  `minquantity` int(10) unsigned DEFAULT '0',
  `category` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `room` (`room`),
  KEY `cat` (`category`),
  CONSTRAINT `cat` FOREIGN KEY (`category`) REFERENCES `categories` (`id`),
  CONSTRAINT `room` FOREIGN KEY (`room`) REFERENCES `rooms` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=70 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rooms`
--

DROP TABLE IF EXISTS `rooms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `rooms` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(50) DEFAULT '''''',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
ALTER TABLE rooms AUTO_INCREMENT = 0;
INSERT INTO rooms (id, name) VALUES (0 ,'Nicht zugeordnet');

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(80) NOT NULL DEFAULT '''''',
  `firstname` varchar(80) NOT NULL DEFAULT '''''',
  `email` varchar(100) NOT NULL DEFAULT '''''',
  `faculty` int(11) NOT NULL DEFAULT '0',
  `telephone` varchar(45) NOT NULL DEFAULT '''''',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=69 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Final view structure for view `allloanpositions_view`
--

/*!50001 DROP TABLE IF EXISTS `allloanpositions_view`*/;
/*!50001 DROP VIEW IF EXISTS `allloanpositions_view`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `allloanpositions_view` AS select `p`.`id` AS `posid`,`p`.`loanid` AS `loanid`,`p`.`quantity` AS `Menge`,`p`.`returnedquantity` AS `Retourmenge`,`r`.`make` AS `Hersteller`,`r`.`type` AS `Typ`,`r`.`id` AS `id` from (`loanposition` `p` join `resources` `r`) where (`p`.`resourceid` = `r`.`id`) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `allloans_view`
--

/*!50001 DROP TABLE IF EXISTS `allloans_view`*/;
/*!50001 DROP VIEW IF EXISTS `allloans_view`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `allloans_view` AS select `l`.`id` AS `loanid`,`u`.`id` AS `userid`,`u`.`name` AS `Name`,`u`.`firstname` AS `Vorname`,`l`.`start` AS `Beginn`,`l`.`end` AS `Ende`,`a`.`username` AS `Mitarbeiter`,`l`.`isactive` AS `isactive` from ((`loans` `l` join `adminuser` `a`) join `users` `u`) where ((`l`.`user` = `u`.`id`) and (`l`.`adminuser` = `a`.`id`)) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `allresources_view`
--

/*!50001 DROP TABLE IF EXISTS `allresources_view`*/;
/*!50001 DROP VIEW IF EXISTS `allresources_view`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `allresources_view` AS select `c`.`name` AS `Kategorie`,`c`.`id` AS `KategorieID`,`rr`.`name` AS `Raum`,`r`.`locker` AS `Schrank`,`r`.`make` AS `Hersteller`,`r`.`type` AS `Typ`,`r`.`description` AS `Beschreibung`,`r`.`id` AS `ResourceID`,`r`.`quantity` AS `Menge`,`r`.`minquantity` AS `MinMenge` from ((`categories` `c` join `resources` `r`) join `rooms` `rr`) where ((`c`.`id` = `r`.`category`) and (`r`.`room` = `rr`.`id`)) order by `c`.`name` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `allroomloans_view`
--

/*!50001 DROP TABLE IF EXISTS `allroomloans_view`*/;
/*!50001 DROP VIEW IF EXISTS `allroomloans_view`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `allroomloans_view` AS select `er`.`id` AS `EditingroomloanID`,`e`.`id` AS `RoomID`,`e`.`name` AS `Raum`,`er`.`start` AS `Start`,`er`.`end` AS `Ende`,`u`.`name` AS `Name`,`u`.`firstname` AS `Vorname`,`u`.`email` AS `Email`,`u`.`telephone` AS `Telefon`,`a`.`username` AS `Mitarbeiter` from (((`users` `u` join `editingroomloans` `er`) join `editingrooms` `e`) join `adminuser` `a`) where ((`er`.`user` = `u`.`id`) and (`er`.`room` = `e`.`id`) and (`er`.`adminuser` = `a`.`id`)) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2014-07-20 23:03:54
