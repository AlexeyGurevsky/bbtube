CREATE TABLE Playlist
(
	PlaylistId INTEGER PRIMARY KEY,
	Type int NOT NULL,
	Title nvarchar(256) NOT NULL,
	DateCreated int NOT NULL,
	DateLastPlayed int NOT NULL,
	YoutubeId varchar(128) NOT NULL,
	ChannelTitle nvarchar(256) NOT NULL
);

INSERT OR IGNORE INTO Playlist(Type, Title, DateCreated, DateLastPlayed, YoutubeId, ChannelTitle)
VALUES(1, 'Favorites', 0, 0, '', '');

INSERT OR IGNORE INTO Playlist(Type, Title, DateCreated, DateLastPlayed, YoutubeId, ChannelTitle)
VALUES(2, 'Watch Later', 0, 0, '', '');

INSERT OR IGNORE INTO Playlist(Type, Title, DateCreated, DateLastPlayed, YoutubeId, ChannelTitle)
VALUES(3, 'History', 0, 0, '', '');
