
########### install files ###############
install(FILES  floor_air_0_0.png floor_air_0_1.png floor_air_0_2.png  	floor_air_1_0.png floor_air_1_1.png floor_air_1_2.png floor_air_2_0.png floor_air_2_1.png  	floor_air_2_2.png floor_carpet_0_0.png floor_carpet_0_1.png floor_carpet_1_0.png  	floor_carpet_1_1.png floor_carpet_2_0.png floor_carpet_2_1.png floor_ceramic_0_0.png  	floor_ceramic_0_1.png floor_ceramic_0_2.png floor_ceramic_1_0.png floor_ceramic_1_1.png  	floor_ceramic_1_2.png floor_ceramic_2_0.png floor_ceramic_2_1.png floor_ceramic_2_2.png  	floor_cobblestone_0_0.png floor_cobblestone_0_1.png floor_cobblestone_0_2.png  	floor_cobblestone_1_0.png floor_cobblestone_1_1.png floor_cobblestone_1_2.png  	floor_cobblestone_2_0.png floor_cobblestone_2_1.png floor_cobblestone_2_2.png  	floor_cobblestone_edge1.png floor_cobblestone_edge10.png floor_cobblestone_edge11.png  	floor_cobblestone_edge12.png floor_cobblestone_edge2.png floor_cobblestone_edge3.png  	floor_cobblestone_edge4.png floor_cobblestone_edge5.png floor_cobblestone_edge6.png  	floor_cobblestone_edge7.png floor_cobblestone_edge8.png floor_cobblestone_edge9.png  	floor_dark_0_0.png floor_highlight.png floor_ice_0_0.png floor_ice_0_1.png  	floor_ice_0_2.png floor_ice_1_0.png floor_ice_1_1.png floor_ice_1_2.png floor_ice_2_0.png  	floor_ice_2_1.png floor_ice_2_2.png floor_invisible.png floor_lava_0_0.png  	floor_lava_0_1.png floor_lava_0_2.png floor_lava_1_0.png floor_lava_1_1.png  	floor_lava_1_2.png floor_lava_2_0.png floor_lava_2_1.png floor_lava_2_2.png  	floor_marble_0_0.png floor_marble_0_1.png floor_marble_0_2.png floor_marble_1_0.png  	floor_marble_1_1.png floor_marble_1_2.png floor_marble_2_0.png floor_marble_2_1.png  	floor_marble_2_2.png floor_moss_covered_0_0.png floor_moss_covered_0_1.png  	floor_moss_covered_0_2.png floor_moss_covered_1_0.png floor_moss_covered_1_1.png  	floor_moss_covered_1_2.png floor_moss_covered_2_0.png floor_moss_covered_2_1.png  	floor_moss_covered_2_2.png floor_mural2_0_0.png floor_mural2_0_1.png floor_mural2_1_0.png  	floor_mural2_1_1.png floor_mural2_2_0.png floor_mural2_2_1.png floor_mural_0_0.png  	floor_mural_0_1.png floor_mural_1_0.png floor_mural_1_1.png floor_mural_2_0.png  	floor_mural_2_1.png floor_rough_0_0.png floor_rough_0_1.png floor_rough_0_2.png  	floor_rough_1_0.png floor_rough_1_1.png floor_rough_1_2.png floor_rough_2_0.png  	floor_rough_2_1.png floor_rough_2_2.png floor_rough_lit_0_0.png floor_rough_lit_0_1.png  	floor_rough_lit_0_2.png floor_rough_lit_1_0.png floor_rough_lit_1_1.png floor_rough_lit_1_2.png  	floor_rough_lit_2_0.png floor_rough_lit_2_1.png floor_rough_lit_2_2.png floor_water_0_0.png  	floor_water_0_1.png floor_water_0_2.png floor_water_1_0.png floor_water_1_1.png  	floor_water_1_2.png floor_water_2_0.png floor_water_2_1.png floor_water_2_2.png DESTINATION share/RPG/data/graphic/floors)

#original Makefile.am contents follow:

#pkgdatadir = $(datadir)/@META_PACKAGE@
#graphicdir = $(pkgdatadir)/data/graphic/floors
#graphic_DATA = floor_air_0_0.png floor_air_0_1.png floor_air_0_2.png \
#	floor_air_1_0.png floor_air_1_1.png floor_air_1_2.png floor_air_2_0.png floor_air_2_1.png \
#	floor_air_2_2.png floor_carpet_0_0.png floor_carpet_0_1.png floor_carpet_1_0.png \
#	floor_carpet_1_1.png floor_carpet_2_0.png floor_carpet_2_1.png floor_ceramic_0_0.png \
#	floor_ceramic_0_1.png floor_ceramic_0_2.png floor_ceramic_1_0.png floor_ceramic_1_1.png \
#	floor_ceramic_1_2.png floor_ceramic_2_0.png floor_ceramic_2_1.png floor_ceramic_2_2.png \
#	floor_cobblestone_0_0.png floor_cobblestone_0_1.png floor_cobblestone_0_2.png \
#	floor_cobblestone_1_0.png floor_cobblestone_1_1.png floor_cobblestone_1_2.png \
#	floor_cobblestone_2_0.png floor_cobblestone_2_1.png floor_cobblestone_2_2.png \
#	floor_cobblestone_edge1.png floor_cobblestone_edge10.png floor_cobblestone_edge11.png \
#	floor_cobblestone_edge12.png floor_cobblestone_edge2.png floor_cobblestone_edge3.png \
#	floor_cobblestone_edge4.png floor_cobblestone_edge5.png floor_cobblestone_edge6.png \
#	floor_cobblestone_edge7.png floor_cobblestone_edge8.png floor_cobblestone_edge9.png \
#	floor_dark_0_0.png floor_highlight.png floor_ice_0_0.png floor_ice_0_1.png \
#	floor_ice_0_2.png floor_ice_1_0.png floor_ice_1_1.png floor_ice_1_2.png floor_ice_2_0.png \
#	floor_ice_2_1.png floor_ice_2_2.png floor_invisible.png floor_lava_0_0.png \
#	floor_lava_0_1.png floor_lava_0_2.png floor_lava_1_0.png floor_lava_1_1.png \
#	floor_lava_1_2.png floor_lava_2_0.png floor_lava_2_1.png floor_lava_2_2.png \
#	floor_marble_0_0.png floor_marble_0_1.png floor_marble_0_2.png floor_marble_1_0.png \
#	floor_marble_1_1.png floor_marble_1_2.png floor_marble_2_0.png floor_marble_2_1.png \
#	floor_marble_2_2.png floor_moss_covered_0_0.png floor_moss_covered_0_1.png \
#	floor_moss_covered_0_2.png floor_moss_covered_1_0.png floor_moss_covered_1_1.png \
#	floor_moss_covered_1_2.png floor_moss_covered_2_0.png floor_moss_covered_2_1.png \
#	floor_moss_covered_2_2.png floor_mural2_0_0.png floor_mural2_0_1.png floor_mural2_1_0.png \
#	floor_mural2_1_1.png floor_mural2_2_0.png floor_mural2_2_1.png floor_mural_0_0.png \
#	floor_mural_0_1.png floor_mural_1_0.png floor_mural_1_1.png floor_mural_2_0.png \
#	floor_mural_2_1.png floor_rough_0_0.png floor_rough_0_1.png floor_rough_0_2.png \
#	floor_rough_1_0.png floor_rough_1_1.png floor_rough_1_2.png floor_rough_2_0.png \
#	floor_rough_2_1.png floor_rough_2_2.png floor_rough_lit_0_0.png floor_rough_lit_0_1.png \
#	floor_rough_lit_0_2.png floor_rough_lit_1_0.png floor_rough_lit_1_1.png floor_rough_lit_1_2.png \
#	floor_rough_lit_2_0.png floor_rough_lit_2_1.png floor_rough_lit_2_2.png floor_water_0_0.png \
#	floor_water_0_1.png floor_water_0_2.png floor_water_1_0.png floor_water_1_1.png \
#	floor_water_1_2.png floor_water_2_0.png floor_water_2_1.png floor_water_2_2.png
#
#EXTRA_DIST = $(graphic_DATA)
