SELECT * FROM DB;
SELECT DISTINCT FROM DB;
SELECT * FROM DB WHERE Col1='Germany' AND (Col2='Berlin' OR Col2='München');
SELECT C1, C2 FROM DB ORDER BY C1 ASC/DESC, C2 ASC/DESC;
INSERT INTO DB (C1, C2, C3, ...) VALUES (val1, val2, val3, ...);
UPDATE DB SET column1=value1,column2=value2,... WHERE some_column=some_value;
DELETE FROM DB WHERE some_column=some_value;
DELETE * FROM DB; (deletes everything)
SELECT TOP number|percent column_name(s) FROM DB;
SELECT * FROM DB WHERE C1 LIKE 's%';   (%land% => has patter land  s% => starts with s %s => ends with s)
SELECT C1,... FROM DB WHERE C1,... IN (value1,value2,...);
SELECT C1,... FROM DB WHERE C2 BETWEEN value1 AND value2; (check how database uses between (inclusive/exclusive))
SELECT * FROM Orders WHERE OrderDate BETWEEN #07/04/1996# AND #07/09/1996#; (for dates)
SELECT column_name AS alias_name FROM table_name;
SELECT column_name(s) FROM table_name AS alias_name;
SELECT CustomerName, Address+', '+City+', '+PostalCode+', '+Country AS Address FROM Customers;
SELECT DB1.C1, DB2.C2, DB1.C3 FROM DB1 INNER JOIN DB2 ON DB1.C=DB2.C;

**********
WILDCARDS
**********
%	A substitute for zero or more characters
_	A substitute for a single character
[charlist]	Sets and ranges of characters to match
[^charlist] Matches only a character NOT specified within the brackets
or
[!charlist] Matches only a character NOT specified within the brackets


************
JOINS
***********
INNER JOIN: Returns all rows when there is at least one match in BOTH tables
LEFT JOIN: Return all rows from the left table, and the matched rows from the right table
RIGHT JOIN: Return all rows from the right table, and the matched rows from the left table
FULL JOIN: Return all rows when there is a match in ONE of the tables

