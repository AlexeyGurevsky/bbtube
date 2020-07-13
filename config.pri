# Config.pri file version 2.0. Auto-generated by IDE. Any changes made by user will be lost!
BASEDIR = $$quote($$_PRO_FILE_PWD_)

device {
    CONFIG(debug, debug|release) {
        profile {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        } else {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }

    }

    CONFIG(release, debug|release) {
        !profile {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }
    }
}

simulator {
    CONFIG(debug, debug|release) {
        !profile {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }
    }
}

config_pri_assets {
    OTHER_FILES += \
        $$quote($$BASEDIR/assets/cc.xml) \
        $$quote($$BASEDIR/assets/images/ic_add_bookmarks.png) \
        $$quote($$BASEDIR/assets/images/ic_backward_10.png) \
        $$quote($$BASEDIR/assets/images/ic_browser.png) \
        $$quote($$BASEDIR/assets/images/ic_cc.png) \
        $$quote($$BASEDIR/assets/images/ic_close.png) \
        $$quote($$BASEDIR/assets/images/ic_contact.png) \
        $$quote($$BASEDIR/assets/images/ic_continue.png) \
        $$quote($$BASEDIR/assets/images/ic_copy_link.png) \
        $$quote($$BASEDIR/assets/images/ic_delete.png) \
        $$quote($$BASEDIR/assets/images/ic_done.png) \
        $$quote($$BASEDIR/assets/images/ic_download.png) \
        $$quote($$BASEDIR/assets/images/ic_equalizer.png) \
        $$quote($$BASEDIR/assets/images/ic_favorite.png) \
        $$quote($$BASEDIR/assets/images/ic_forward_10.png) \
        $$quote($$BASEDIR/assets/images/ic_import.png) \
        $$quote($$BASEDIR/assets/images/ic_info.png) \
        $$quote($$BASEDIR/assets/images/ic_later.png) \
        $$quote($$BASEDIR/assets/images/ic_local.png) \
        $$quote($$BASEDIR/assets/images/ic_next.png) \
        $$quote($$BASEDIR/assets/images/ic_open.png) \
        $$quote($$BASEDIR/assets/images/ic_open_link.png) \
        $$quote($$BASEDIR/assets/images/ic_pause.png) \
        $$quote($$BASEDIR/assets/images/ic_paypal.png) \
        $$quote($$BASEDIR/assets/images/ic_play.png) \
        $$quote($$BASEDIR/assets/images/ic_playlists.png) \
        $$quote($$BASEDIR/assets/images/ic_previous.png) \
        $$quote($$BASEDIR/assets/images/ic_recommended.png) \
        $$quote($$BASEDIR/assets/images/ic_reload.png) \
        $$quote($$BASEDIR/assets/images/ic_remote.png) \
        $$quote($$BASEDIR/assets/images/ic_remove_bookmarks.png) \
        $$quote($$BASEDIR/assets/images/ic_remove_later.png) \
        $$quote($$BASEDIR/assets/images/ic_repeat_all.png) \
        $$quote($$BASEDIR/assets/images/ic_repeat_off.png) \
        $$quote($$BASEDIR/assets/images/ic_repeat_one.png) \
        $$quote($$BASEDIR/assets/images/ic_rss.png) \
        $$quote($$BASEDIR/assets/images/ic_scaling.png) \
        $$quote($$BASEDIR/assets/images/ic_search.png) \
        $$quote($$BASEDIR/assets/images/ic_select.png) \
        $$quote($$BASEDIR/assets/images/ic_share.png) \
        $$quote($$BASEDIR/assets/images/ic_shuffle.png) \
        $$quote($$BASEDIR/assets/images/ic_sort.png) \
        $$quote($$BASEDIR/assets/images/ic_speaker_dk.png) \
        $$quote($$BASEDIR/assets/images/ic_stop.png) \
        $$quote($$BASEDIR/assets/images/ic_timecode.png) \
        $$quote($$BASEDIR/assets/images/ic_to_bottom.png) \
        $$quote($$BASEDIR/assets/images/ic_to_top.png) \
        $$quote($$BASEDIR/assets/images/ic_trending.png) \
        $$quote($$BASEDIR/assets/images/ic_view_list.png) \
        $$quote($$BASEDIR/assets/images/player_background.png) \
        $$quote($$BASEDIR/assets/images/player_background_mini.png) \
        $$quote($$BASEDIR/assets/sql/Script000001.sql) \
        $$quote($$BASEDIR/assets/sql/Script000002.sql) \
        $$quote($$BASEDIR/assets/sql/Script000003.sql) \
        $$quote($$BASEDIR/assets/sql/Script000004.sql) \
        $$quote($$BASEDIR/assets/sql/Script000005.sql) \
        $$quote($$BASEDIR/assets/sql/Script000006.sql) \
        $$quote($$BASEDIR/assets/youtube.db)
}

config_pri_source_group1 {
    SOURCES += \
        $$quote($$BASEDIR/src/BrowseTab/BrowseTab.cpp) \
        $$quote($$BASEDIR/src/ChannelListPage/ChannelListItem.cpp) \
        $$quote($$BASEDIR/src/ChannelListPage/ChannelListPage.cpp) \
        $$quote($$BASEDIR/src/ChannelListPage/ImportChannelsSheet.cpp) \
        $$quote($$BASEDIR/src/ChannelListPage/SearchChannelsSheet.cpp) \
        $$quote($$BASEDIR/src/ChannelPage/ChannelPage.cpp) \
        $$quote($$BASEDIR/src/ChannelsTab/ChannelsTab.cpp) \
        $$quote($$BASEDIR/src/Cover/AppCover.cpp) \
        $$quote($$BASEDIR/src/Equalizer/EqualizerListItem.cpp) \
        $$quote($$BASEDIR/src/Equalizer/EqualizerSheet.cpp) \
        $$quote($$BASEDIR/src/PlayerPage/PlayerPage.cpp) \
        $$quote($$BASEDIR/src/PlayerPage/TimecodeSheet.cpp) \
        $$quote($$BASEDIR/src/PlaylistPage/PlaylistPage.cpp) \
        $$quote($$BASEDIR/src/PlaylistPage/PlaylistVideoListItem.cpp) \
        $$quote($$BASEDIR/src/PlaylistPage/SearchVideosSheet.cpp) \
        $$quote($$BASEDIR/src/PlaylistsPage/PlaylistsPage.cpp) \
        $$quote($$BASEDIR/src/PlaylistsTab/PlaylistsTab.cpp) \
        $$quote($$BASEDIR/src/RecommendedTab/RecommendedPage.cpp) \
        $$quote($$BASEDIR/src/RecommendedTab/RecommendedTab.cpp) \
        $$quote($$BASEDIR/src/StartPage/StartPage.cpp) \
        $$quote($$BASEDIR/src/TrendingTab/TrendingPage.cpp) \
        $$quote($$BASEDIR/src/TrendingTab/TrendingTab.cpp) \
        $$quote($$BASEDIR/src/VideoList/SearchListItem.cpp) \
        $$quote($$BASEDIR/src/applicationui.cpp) \
        $$quote($$BASEDIR/src/db/DbHelper.cpp) \
        $$quote($$BASEDIR/src/info/InfoSheet.cpp) \
        $$quote($$BASEDIR/src/main.cpp) \
        $$quote($$BASEDIR/src/menu/AppMenu.cpp) \
        $$quote($$BASEDIR/src/parser/YoutubeClient.cpp) \
        $$quote($$BASEDIR/src/parser/channel/ChannelPageParser.cpp) \
        $$quote($$BASEDIR/src/parser/cipher/DecryptHelper.cpp) \
        $$quote($$BASEDIR/src/parser/recommended/RecommendedPageParser.cpp) \
        $$quote($$BASEDIR/src/parser/script/ScriptParser.cpp) \
        $$quote($$BASEDIR/src/parser/storage/StorageParser.cpp) \
        $$quote($$BASEDIR/src/parser/trending/TrendingPageParser.cpp) \
        $$quote($$BASEDIR/src/settings/SettingsSheet.cpp) \
        $$quote($$BASEDIR/src/utils/ActionItemService.cpp) \
        $$quote($$BASEDIR/src/utils/BasePage.cpp) \
        $$quote($$BASEDIR/src/utils/BaseSheet.cpp) \
        $$quote($$BASEDIR/src/utils/ChannelListProxy.cpp) \
        $$quote($$BASEDIR/src/utils/GlobalPlayerContext.cpp) \
        $$quote($$BASEDIR/src/utils/MiniPlayer.cpp) \
        $$quote($$BASEDIR/src/utils/PlaylistSheet.cpp) \
        $$quote($$BASEDIR/src/utils/PlaylistVideoProxy.cpp) \
        $$quote($$BASEDIR/src/utils/UIUtils.cpp) \
        $$quote($$BASEDIR/src/utils/VideoViewedPercentProxy.cpp)

    HEADERS += \
        $$quote($$BASEDIR/src/BrowseTab/BrowseTab.hpp) \
        $$quote($$BASEDIR/src/ChannelListPage/ChannelListItemProvider.hpp) \
        $$quote($$BASEDIR/src/ChannelListPage/ChannelListPage.hpp) \
        $$quote($$BASEDIR/src/ChannelListPage/ImportChannelsSheet.hpp) \
        $$quote($$BASEDIR/src/ChannelListPage/RssVideoModel.hpp) \
        $$quote($$BASEDIR/src/ChannelListPage/SearchChannelsSheet.hpp) \
        $$quote($$BASEDIR/src/ChannelPage/ChannelPage.hpp) \
        $$quote($$BASEDIR/src/ChannelsTab/ChannelsTab.hpp) \
        $$quote($$BASEDIR/src/Cover/AppCover.hpp) \
        $$quote($$BASEDIR/src/Equalizer/EqualizerListItem.hpp) \
        $$quote($$BASEDIR/src/Equalizer/EqualizerListItemModel.hpp) \
        $$quote($$BASEDIR/src/Equalizer/EqualizerListItemProvider.hpp) \
        $$quote($$BASEDIR/src/Equalizer/EqualizerSheet.hpp) \
        $$quote($$BASEDIR/src/PlayerPage/PlayerPage.hpp) \
        $$quote($$BASEDIR/src/PlayerPage/TimecodeListItemModel.hpp) \
        $$quote($$BASEDIR/src/PlayerPage/TimecodeListItemProvider.hpp) \
        $$quote($$BASEDIR/src/PlayerPage/TimecodeSheet.hpp) \
        $$quote($$BASEDIR/src/PlaylistPage/PlaylistPage.hpp) \
        $$quote($$BASEDIR/src/PlaylistPage/PlaylistVideoListItemProvider.hpp) \
        $$quote($$BASEDIR/src/PlaylistPage/SearchVideosSheet.hpp) \
        $$quote($$BASEDIR/src/PlaylistsPage/PlaylistListItemProvider.hpp) \
        $$quote($$BASEDIR/src/PlaylistsPage/PlaylistsPage.hpp) \
        $$quote($$BASEDIR/src/PlaylistsTab/PlaylistsTab.hpp) \
        $$quote($$BASEDIR/src/RecommendedTab/RecommendedPage.hpp) \
        $$quote($$BASEDIR/src/RecommendedTab/RecommendedTab.hpp) \
        $$quote($$BASEDIR/src/StartPage/StartPage.hpp) \
        $$quote($$BASEDIR/src/SuggestionsList/SuggestionsListItemProvider.hpp) \
        $$quote($$BASEDIR/src/TrendingTab/TrendingPage.hpp) \
        $$quote($$BASEDIR/src/TrendingTab/TrendingTab.hpp) \
        $$quote($$BASEDIR/src/VideoList/SearchListItemProvider.hpp) \
        $$quote($$BASEDIR/src/applicationui.hpp) \
        $$quote($$BASEDIR/src/db/DbHelper.hpp) \
        $$quote($$BASEDIR/src/info/InfoSheet.hpp) \
        $$quote($$BASEDIR/src/menu/AppMenu.hpp) \
        $$quote($$BASEDIR/src/models/ChannelListItemModel.hpp) \
        $$quote($$BASEDIR/src/models/PlaylistListItemModel.hpp) \
        $$quote($$BASEDIR/src/models/PlaylistVideoModel.hpp) \
        $$quote($$BASEDIR/src/models/VideoListItemModel.hpp) \
        $$quote($$BASEDIR/src/parser/YoutubeClient.hpp) \
        $$quote($$BASEDIR/src/parser/channel/ChannelPageParser.hpp) \
        $$quote($$BASEDIR/src/parser/cipher/CipherOperations.hpp) \
        $$quote($$BASEDIR/src/parser/cipher/DecryptHelper.hpp) \
        $$quote($$BASEDIR/src/parser/models/ChannelData.hpp) \
        $$quote($$BASEDIR/src/parser/models/RecommendedData.hpp) \
        $$quote($$BASEDIR/src/parser/models/SearchData.hpp) \
        $$quote($$BASEDIR/src/parser/models/StorageData.hpp) \
        $$quote($$BASEDIR/src/parser/models/TrendingData.hpp) \
        $$quote($$BASEDIR/src/parser/models/VideoMetadata.hpp) \
        $$quote($$BASEDIR/src/parser/recommended/RecommendedPageParser.hpp) \
        $$quote($$BASEDIR/src/parser/script/ScriptData.hpp) \
        $$quote($$BASEDIR/src/parser/script/ScriptParser.hpp) \
        $$quote($$BASEDIR/src/parser/search/ItemRendererParser.hpp) \
        $$quote($$BASEDIR/src/parser/search/SuggestionsParser.hpp) \
        $$quote($$BASEDIR/src/parser/storage/StorageParser.hpp) \
        $$quote($$BASEDIR/src/parser/trending/TrendingPageParser.hpp) \
        $$quote($$BASEDIR/src/settings/AppSettings.hpp) \
        $$quote($$BASEDIR/src/settings/SettingsSheet.hpp) \
        $$quote($$BASEDIR/src/utils/ActionItemService.hpp) \
        $$quote($$BASEDIR/src/utils/BasePage.hpp) \
        $$quote($$BASEDIR/src/utils/BaseSheet.hpp) \
        $$quote($$BASEDIR/src/utils/BaseTab.hpp) \
        $$quote($$BASEDIR/src/utils/CCUtils.hpp) \
        $$quote($$BASEDIR/src/utils/ChannelListProxy.hpp) \
        $$quote($$BASEDIR/src/utils/CustomListView.hpp) \
        $$quote($$BASEDIR/src/utils/DateTimeUtils.hpp) \
        $$quote($$BASEDIR/src/utils/GlobalPlayerContext.hpp) \
        $$quote($$BASEDIR/src/utils/MiniPlayer.hpp) \
        $$quote($$BASEDIR/src/utils/PlaybackTimeoutHandler.hpp) \
        $$quote($$BASEDIR/src/utils/PlaylistSheet.hpp) \
        $$quote($$BASEDIR/src/utils/PlaylistVideoProxy.hpp) \
        $$quote($$BASEDIR/src/utils/UIUtils.hpp) \
        $$quote($$BASEDIR/src/utils/UpdatableDataModel.hpp) \
        $$quote($$BASEDIR/src/utils/VideoViewedPercentProxy.hpp)
}

CONFIG += precompile_header

PRECOMPILED_HEADER = $$quote($$BASEDIR/precompiled.h)

lupdate_inclusion {
    SOURCES += \
        $$quote($$BASEDIR/../src/*.c) \
        $$quote($$BASEDIR/../src/*.c++) \
        $$quote($$BASEDIR/../src/*.cc) \
        $$quote($$BASEDIR/../src/*.cpp) \
        $$quote($$BASEDIR/../src/*.cxx) \
        $$quote($$BASEDIR/../src/BrowseTab/*.c) \
        $$quote($$BASEDIR/../src/BrowseTab/*.c++) \
        $$quote($$BASEDIR/../src/BrowseTab/*.cc) \
        $$quote($$BASEDIR/../src/BrowseTab/*.cpp) \
        $$quote($$BASEDIR/../src/BrowseTab/*.cxx) \
        $$quote($$BASEDIR/../src/ChannelListPage/*.c) \
        $$quote($$BASEDIR/../src/ChannelListPage/*.c++) \
        $$quote($$BASEDIR/../src/ChannelListPage/*.cc) \
        $$quote($$BASEDIR/../src/ChannelListPage/*.cpp) \
        $$quote($$BASEDIR/../src/ChannelListPage/*.cxx) \
        $$quote($$BASEDIR/../src/ChannelPage/*.c) \
        $$quote($$BASEDIR/../src/ChannelPage/*.c++) \
        $$quote($$BASEDIR/../src/ChannelPage/*.cc) \
        $$quote($$BASEDIR/../src/ChannelPage/*.cpp) \
        $$quote($$BASEDIR/../src/ChannelPage/*.cxx) \
        $$quote($$BASEDIR/../src/ChannelsTab/*.c) \
        $$quote($$BASEDIR/../src/ChannelsTab/*.c++) \
        $$quote($$BASEDIR/../src/ChannelsTab/*.cc) \
        $$quote($$BASEDIR/../src/ChannelsTab/*.cpp) \
        $$quote($$BASEDIR/../src/ChannelsTab/*.cxx) \
        $$quote($$BASEDIR/../src/Cover/*.c) \
        $$quote($$BASEDIR/../src/Cover/*.c++) \
        $$quote($$BASEDIR/../src/Cover/*.cc) \
        $$quote($$BASEDIR/../src/Cover/*.cpp) \
        $$quote($$BASEDIR/../src/Cover/*.cxx) \
        $$quote($$BASEDIR/../src/Equalizer/*.c) \
        $$quote($$BASEDIR/../src/Equalizer/*.c++) \
        $$quote($$BASEDIR/../src/Equalizer/*.cc) \
        $$quote($$BASEDIR/../src/Equalizer/*.cpp) \
        $$quote($$BASEDIR/../src/Equalizer/*.cxx) \
        $$quote($$BASEDIR/../src/PlayerPage/*.c) \
        $$quote($$BASEDIR/../src/PlayerPage/*.c++) \
        $$quote($$BASEDIR/../src/PlayerPage/*.cc) \
        $$quote($$BASEDIR/../src/PlayerPage/*.cpp) \
        $$quote($$BASEDIR/../src/PlayerPage/*.cxx) \
        $$quote($$BASEDIR/../src/PlaylistPage/*.c) \
        $$quote($$BASEDIR/../src/PlaylistPage/*.c++) \
        $$quote($$BASEDIR/../src/PlaylistPage/*.cc) \
        $$quote($$BASEDIR/../src/PlaylistPage/*.cpp) \
        $$quote($$BASEDIR/../src/PlaylistPage/*.cxx) \
        $$quote($$BASEDIR/../src/PlaylistsPage/*.c) \
        $$quote($$BASEDIR/../src/PlaylistsPage/*.c++) \
        $$quote($$BASEDIR/../src/PlaylistsPage/*.cc) \
        $$quote($$BASEDIR/../src/PlaylistsPage/*.cpp) \
        $$quote($$BASEDIR/../src/PlaylistsPage/*.cxx) \
        $$quote($$BASEDIR/../src/PlaylistsTab/*.c) \
        $$quote($$BASEDIR/../src/PlaylistsTab/*.c++) \
        $$quote($$BASEDIR/../src/PlaylistsTab/*.cc) \
        $$quote($$BASEDIR/../src/PlaylistsTab/*.cpp) \
        $$quote($$BASEDIR/../src/PlaylistsTab/*.cxx) \
        $$quote($$BASEDIR/../src/RecommendedTab/*.c) \
        $$quote($$BASEDIR/../src/RecommendedTab/*.c++) \
        $$quote($$BASEDIR/../src/RecommendedTab/*.cc) \
        $$quote($$BASEDIR/../src/RecommendedTab/*.cpp) \
        $$quote($$BASEDIR/../src/RecommendedTab/*.cxx) \
        $$quote($$BASEDIR/../src/StartPage/*.c) \
        $$quote($$BASEDIR/../src/StartPage/*.c++) \
        $$quote($$BASEDIR/../src/StartPage/*.cc) \
        $$quote($$BASEDIR/../src/StartPage/*.cpp) \
        $$quote($$BASEDIR/../src/StartPage/*.cxx) \
        $$quote($$BASEDIR/../src/SuggestionsList/*.c) \
        $$quote($$BASEDIR/../src/SuggestionsList/*.c++) \
        $$quote($$BASEDIR/../src/SuggestionsList/*.cc) \
        $$quote($$BASEDIR/../src/SuggestionsList/*.cpp) \
        $$quote($$BASEDIR/../src/SuggestionsList/*.cxx) \
        $$quote($$BASEDIR/../src/TrendingTab/*.c) \
        $$quote($$BASEDIR/../src/TrendingTab/*.c++) \
        $$quote($$BASEDIR/../src/TrendingTab/*.cc) \
        $$quote($$BASEDIR/../src/TrendingTab/*.cpp) \
        $$quote($$BASEDIR/../src/TrendingTab/*.cxx) \
        $$quote($$BASEDIR/../src/VideoList/*.c) \
        $$quote($$BASEDIR/../src/VideoList/*.c++) \
        $$quote($$BASEDIR/../src/VideoList/*.cc) \
        $$quote($$BASEDIR/../src/VideoList/*.cpp) \
        $$quote($$BASEDIR/../src/VideoList/*.cxx) \
        $$quote($$BASEDIR/../src/db/*.c) \
        $$quote($$BASEDIR/../src/db/*.c++) \
        $$quote($$BASEDIR/../src/db/*.cc) \
        $$quote($$BASEDIR/../src/db/*.cpp) \
        $$quote($$BASEDIR/../src/db/*.cxx) \
        $$quote($$BASEDIR/../src/info/*.c) \
        $$quote($$BASEDIR/../src/info/*.c++) \
        $$quote($$BASEDIR/../src/info/*.cc) \
        $$quote($$BASEDIR/../src/info/*.cpp) \
        $$quote($$BASEDIR/../src/info/*.cxx) \
        $$quote($$BASEDIR/../src/menu/*.c) \
        $$quote($$BASEDIR/../src/menu/*.c++) \
        $$quote($$BASEDIR/../src/menu/*.cc) \
        $$quote($$BASEDIR/../src/menu/*.cpp) \
        $$quote($$BASEDIR/../src/menu/*.cxx) \
        $$quote($$BASEDIR/../src/models/*.c) \
        $$quote($$BASEDIR/../src/models/*.c++) \
        $$quote($$BASEDIR/../src/models/*.cc) \
        $$quote($$BASEDIR/../src/models/*.cpp) \
        $$quote($$BASEDIR/../src/models/*.cxx) \
        $$quote($$BASEDIR/../src/parser/*.c) \
        $$quote($$BASEDIR/../src/parser/*.c++) \
        $$quote($$BASEDIR/../src/parser/*.cc) \
        $$quote($$BASEDIR/../src/parser/*.cpp) \
        $$quote($$BASEDIR/../src/parser/*.cxx) \
        $$quote($$BASEDIR/../src/parser/channel/*.c) \
        $$quote($$BASEDIR/../src/parser/channel/*.c++) \
        $$quote($$BASEDIR/../src/parser/channel/*.cc) \
        $$quote($$BASEDIR/../src/parser/channel/*.cpp) \
        $$quote($$BASEDIR/../src/parser/channel/*.cxx) \
        $$quote($$BASEDIR/../src/parser/cipher/*.c) \
        $$quote($$BASEDIR/../src/parser/cipher/*.c++) \
        $$quote($$BASEDIR/../src/parser/cipher/*.cc) \
        $$quote($$BASEDIR/../src/parser/cipher/*.cpp) \
        $$quote($$BASEDIR/../src/parser/cipher/*.cxx) \
        $$quote($$BASEDIR/../src/parser/models/*.c) \
        $$quote($$BASEDIR/../src/parser/models/*.c++) \
        $$quote($$BASEDIR/../src/parser/models/*.cc) \
        $$quote($$BASEDIR/../src/parser/models/*.cpp) \
        $$quote($$BASEDIR/../src/parser/models/*.cxx) \
        $$quote($$BASEDIR/../src/parser/recommended/*.c) \
        $$quote($$BASEDIR/../src/parser/recommended/*.c++) \
        $$quote($$BASEDIR/../src/parser/recommended/*.cc) \
        $$quote($$BASEDIR/../src/parser/recommended/*.cpp) \
        $$quote($$BASEDIR/../src/parser/recommended/*.cxx) \
        $$quote($$BASEDIR/../src/parser/script/*.c) \
        $$quote($$BASEDIR/../src/parser/script/*.c++) \
        $$quote($$BASEDIR/../src/parser/script/*.cc) \
        $$quote($$BASEDIR/../src/parser/script/*.cpp) \
        $$quote($$BASEDIR/../src/parser/script/*.cxx) \
        $$quote($$BASEDIR/../src/parser/search/*.c) \
        $$quote($$BASEDIR/../src/parser/search/*.c++) \
        $$quote($$BASEDIR/../src/parser/search/*.cc) \
        $$quote($$BASEDIR/../src/parser/search/*.cpp) \
        $$quote($$BASEDIR/../src/parser/search/*.cxx) \
        $$quote($$BASEDIR/../src/parser/storage/*.c) \
        $$quote($$BASEDIR/../src/parser/storage/*.c++) \
        $$quote($$BASEDIR/../src/parser/storage/*.cc) \
        $$quote($$BASEDIR/../src/parser/storage/*.cpp) \
        $$quote($$BASEDIR/../src/parser/storage/*.cxx) \
        $$quote($$BASEDIR/../src/parser/trending/*.c) \
        $$quote($$BASEDIR/../src/parser/trending/*.c++) \
        $$quote($$BASEDIR/../src/parser/trending/*.cc) \
        $$quote($$BASEDIR/../src/parser/trending/*.cpp) \
        $$quote($$BASEDIR/../src/parser/trending/*.cxx) \
        $$quote($$BASEDIR/../src/settings/*.c) \
        $$quote($$BASEDIR/../src/settings/*.c++) \
        $$quote($$BASEDIR/../src/settings/*.cc) \
        $$quote($$BASEDIR/../src/settings/*.cpp) \
        $$quote($$BASEDIR/../src/settings/*.cxx) \
        $$quote($$BASEDIR/../src/utils/*.c) \
        $$quote($$BASEDIR/../src/utils/*.c++) \
        $$quote($$BASEDIR/../src/utils/*.cc) \
        $$quote($$BASEDIR/../src/utils/*.cpp) \
        $$quote($$BASEDIR/../src/utils/*.cxx) \
        $$quote($$BASEDIR/../assets/*.qml) \
        $$quote($$BASEDIR/../assets/*.js) \
        $$quote($$BASEDIR/../assets/*.qs) \
        $$quote($$BASEDIR/../assets/images/*.qml) \
        $$quote($$BASEDIR/../assets/images/*.js) \
        $$quote($$BASEDIR/../assets/images/*.qs) \
        $$quote($$BASEDIR/../assets/sql/*.qml) \
        $$quote($$BASEDIR/../assets/sql/*.js) \
        $$quote($$BASEDIR/../assets/sql/*.qs)

    HEADERS += \
        $$quote($$BASEDIR/../src/*.h) \
        $$quote($$BASEDIR/../src/*.h++) \
        $$quote($$BASEDIR/../src/*.hh) \
        $$quote($$BASEDIR/../src/*.hpp) \
        $$quote($$BASEDIR/../src/*.hxx)
}

TRANSLATIONS = $$quote($${TARGET}.ts)