DELIMITER //
CREATE TRIGGER insert_send
BEFORE INSERT ON send
FOR EACH ROW
BEGIN
    IF NOT EXISTS (SELECT * FROM user WHERE iduser = NEW.iduser) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Invalid user id';
    END IF;
END;//


DELIMITER //
CREATE TRIGGER delete_send
BEFORE DELETE ON send
FOR EACH ROW
BEGIN
    IF NOT EXISTS (SELECT * FROM send WHERE iduser = OLD.iduser) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Cannot delete due to foreign key constraint';
    END IF;
END;//
DELIMITER //
CREATE TRIGGER update_send
BEFORE UPDATE ON send
FOR EACH ROW
BEGIN
    IF NOT EXISTS (SELECT * FROM user WHERE iduser = NEW.iduser) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Invalid user id in new data';
    END IF;
END;//
DELIMITER //
CREATE TRIGGER insert_receive
BEFORE INSERT ON receive
FOR EACH ROW
BEGIN
    IF NOT EXISTS (SELECT * FROM user WHERE iduser = NEW.iduser) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Invalid user id';
    END IF;
END;//
DELIMITER //
CREATE TRIGGER delete_receive
BEFORE DELETE ON receive
FOR EACH ROW
BEGIN
    IF not EXISTS (SELECT * FROM send WHERE iduser = OLD.iduser) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Cannot delete due to foreign key constraint';
    END IF;
END;//
DELIMITER //
drop trigger delete_manage;
//
DELIMITER //
CREATE TRIGGER update_receive
BEFORE UPDATE ON receive
FOR EACH ROW
BEGIN
    IF NOT EXISTS (SELECT * FROM user WHERE iduser = NEW.iduser) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Invalid user id in new data';
    END IF;
END;//
DELIMITER //
CREATE TRIGGER insert_manage
BEFORE INSERT ON manage
FOR EACH ROW
BEGIN
    IF NOT EXISTS (SELECT * FROM user WHERE iduser = NEW.iduser) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Invalid user id';
    END IF;
END;//
CREATE TRIGGER delete_manage
BEFORE DELETE ON manage
FOR EACH ROW
BEGIN
    IF not EXISTS (SELECT * FROM manage WHERE iduser = OLD.iduser) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Cannot delete due to foreign key constraint';
    END IF;
END;//
DELIMITER //
CREATE TRIGGER update_manage
BEFORE UPDATE ON manage
FOR EACH ROW
BEGIN
    IF NOT EXISTS (SELECT * FROM user WHERE iduser = NEW.iduser) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Invalid user id in new data';
    END IF;
END;//
CREATE TRIGGER insert_ca
BEFORE INSERT ON ca
FOR EACH ROW
    begin
END;//
DELIMITER //
CREATE TRIGGER delete_ca
BEFORE DELETE ON ca
FOR EACH ROW
BEGIN   
END;//
DELIMITER //
CREATE TRIGGER update_ca
BEFORE UPDATE ON ca
FOR EACH ROW
BEGIN
END;//
DELIMITER //
CREATE TRIGGER insert_user
BEFORE INSERT ON user
FOR EACH ROW
BEGIN
    
END;//
CREATE TRIGGER delete_user
BEFORE DELETE ON user
FOR EACH ROW
BEGIN
    
END;//
CREATE TRIGGER update_user
BEFORE UPDATE ON user
FOR EACH ROW
BEGIN
    
    
END;//
DELIMITER //
drop trigger delete_user
