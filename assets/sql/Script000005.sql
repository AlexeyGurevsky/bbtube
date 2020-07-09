CREATE TABLE PlaylistVideo
(
	VideoId varchar(32) NOT NULL,
	PlaylistId int NOT NULL,
	Title nvarchar(256) NOT NULL,
	ChannelId varchar(32) NOT NULL,
	ChannelTitle nvarchar(256) NOT NULL,
	DateAdded int NOT NULL,
	DateLastPlayed int NOT NULL,
	SortOrder int NOT NULL,
	CONSTRAINT PK_PlaylistVideo PRIMARY KEY
	(
		VideoId ASC,
		PlaylistId ASC
	)
);