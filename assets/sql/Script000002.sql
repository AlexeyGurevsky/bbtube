CREATE TABLE Channel
(
	ChannelId varchar(32) NOT NULL,
	Title nvarchar(256) NOT NULL,
	ThumbnailUrl varchar(256) NOT NULL,
	DateAdded int NOT NULL,
	DateLastVisited int NOT NULL,
	LastVideoId varchar(32) NOT NULL,
	LastVideoTitle nvarchar(1024) NOT NULL,
	DateLastActivity int NOT NULL,
	CONSTRAINT PK_Channel PRIMARY KEY
	(
		ChannelId ASC
	)
);