CREATE TABLE Settings
(
	[Key] varchar(128),
	[Value] varchar(128),
	CONSTRAINT PK_Settings PRIMARY KEY
	(
		[Key] ASC
	)
);

INSERT INTO Settings([Key], [Value])
VALUES('autoplay', '1');