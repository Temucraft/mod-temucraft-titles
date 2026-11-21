-- mod-temucraft-titles: its game master commands as RBAC permissions (101401-101404). Each is linked to the
-- stock security role of its old level (194 moderator, 193 game master, 192 administrator) and to the matching
-- Temucraft GM tier (1011 Triager, 1012 Entertainer, 1014 Administrator). Grant or deny them per account with
-- .rbac account grant / deny, or link them to other roles.
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` BETWEEN 101401 AND 101449;
DELETE FROM `rbac_permissions` WHERE `id` BETWEEN 101401 AND 101449;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(101401, 'Command: temucraft temutitle find'),
(101402, 'Command: temucraft temutitle give'),
(101403, 'Command: temucraft temutitle take'),
(101404, 'Command: temucraft temutitle reload');
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(192, 101401),
(1014, 101401),
(192, 101402),
(1014, 101402),
(192, 101403),
(1014, 101403),
(192, 101404),
(1014, 101404);
