########### install files ###############
install (FILES
         image_icon.png
         image_main_logo.png
         interface_elements.png
         interface_minimap_bg.png
         DESTINATION ${INSTALL_SHARE_DIR}/images COMPONENT data)
if (WIN32)
 install (FILES
          image_icon.ico
          DESTINATION ${INSTALL_INCLUDE_DIR} COMPONENT config)
endif (WIN32)
