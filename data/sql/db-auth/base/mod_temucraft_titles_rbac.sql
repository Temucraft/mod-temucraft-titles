-- mod-temucraft-titles: its game master commands as RBAC permissions (101401-101404). Each is linked to the
-- stock security role of its old level (194 moderator, 193 game master, 192 administrator) and to the matching
-- Temucraft GM tier (1011 Triager, 1012 Entertainer, 1014 Administrator). Grant or deny them per account with
-- .rbac account grant / deny, or link them to other roles.
DELETE FROM `rbac_linked_permissions` WHERE `linkedId` BETWEEN 101401 AND 101499;
DELETE FROM `rbac_permissions` WHERE `id` BETWEEN 101401 AND 101499;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(101401, 'Command: temutitle find'),
(101402, 'Command: temutitle give'),
(101403, 'Command: temutitle take'),
(101404, 'Command: temutitle reload');
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(193, 101401),
(1012, 101401),
(193, 101402),
(1012, 101402),
(193, 101403),
(1012, 101403),
(192, 101404),
(1014, 101404);
