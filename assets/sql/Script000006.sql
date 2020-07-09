CREATE TABLE VideoViewedPercent
(
	VideoId varchar(32) NOT NULL,
	Percent int NOT NULL,
	CONSTRAINT PK_VideoViewedPercent PRIMARY KEY
	(
		VideoId ASC
	)
);