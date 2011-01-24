PROJECT(kopete_xfire)

find_package(KDE4 REQUIRED)
find_package(Qt4 REQUIRED)

INCLUDE_DIRECTORIES(${QT4_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${QT_QTCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${QT_QTGUI_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${QT_QTNETWORK_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${KDE4_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${KDE4_INCLUDES})

include(KDE4Defaults)
include(MacroLibrary)

find_path(KOPETE_INCLUDE_DIR
	NAME kopeteversion.h
	PATH_SUFFIXES kopete
	HINTS
	${INCLUDE_INSTALL_DIR}
	${KDE4_INCLUDE_DIR}
)

add_subdirectory(icons)

include_directories(
	${KOPETE_INCLUDE_DIR}
	${KOPETE_INCLUDE_DIR}/ui
	${CMAKE_CURRENT_SOURCE_DIR}/libxfire
)

set(kopete_xfire_PART_SRCS
	src/xf_account.cpp
	src/xf_add_contact.cpp
	src/xf_contact.cpp
	src/xf_protocol.cpp
	src/xf_server.cpp
	src/xf_edit_account.cpp
	src/xf_games_list.cpp
	src/xf_game_detection.cpp
	src/xf_games_manager.cpp
	src/xf_p2p.cpp
	src/xf_p2p_session.cpp
	src/xf_p2p_natcheck.cpp

	libxfire/XfireAttribute.cpp
	libxfire/XfirePacket.cpp
     libxfire/XfirePeerToPeerPacket.cpp
	libxfire/XfireTypes.cpp
)

kde4_add_ui_files(kopete_xfire_PART_SRCS
	ui/add_contact.ui
	ui/edit_account.ui
	ui/edit_games.ui
)

kde4_add_plugin(kopete_xfire ${kopete_xfire_PART_SRCS})
target_link_libraries(kopete_xfire ${KDE4_KIO_LIBS} kopete
${QT_QTNETWORK_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_CORE_LIBRARY})

########### install files ###############

install(TARGETS kopete_xfire  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES src/kopete_xfire.desktop DESTINATION ${SERVICES_INSTALL_DIR})
