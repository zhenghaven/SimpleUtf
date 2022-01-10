option(SIMPLEUTF_INSTALL_HEADERS "Install SimpleUtf headers." ON)

if(SIMPLEUTF_INSTALL_HEADERS)

	file(GLOB headers "SimpleUtf/*.hpp")

	install(FILES ${headers}
		DESTINATION include/SimpleUtf
		PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

	#add_subdirectory(SimpleUtf)

endif(SIMPLEUTF_INSTALL_HEADERS)
