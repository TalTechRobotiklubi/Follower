echo off
SET current_dir=%~dp0

SET source_path=%current_dir%..\SensorBoard\src\
SET destination_path=%current_dir%\src\

echo %source_path%
echo %destination_path%

copy %source_path%SystemTicks.h %destination_path%SystemTicks.h
copy %source_path%SystemTicks.c %destination_path%SystemTicks.c
copy %source_path%TaskHandler.h %destination_path%TaskHandler.h
copy %source_path%TaskHandler.c %destination_path%TaskHandler.c
copy %source_path%DataLayer.h %destination_path%DataLayer.h
copy %source_path%DataLayer.c %destination_path%DataLayer.c
copy %source_path%DataLayerConfig.h %destination_path%DataLayerConfig.h
copy %source_path%DataLayerConfig.c %destination_path%DataLayerConfig.c
copy %source_path%PacketConfig.h %destination_path%PacketConfig.h
copy %source_path%PacketConfig.c %destination_path%PacketConfig.c
copy %source_path%InterfaceConfig.h %destination_path%InterfaceConfig.h
copy %source_path%InterfaceConfig.c %destination_path%InterfaceConfig.c
copy %source_path%InterfaceHandler.h %destination_path%InterfaceHandler.h
copy %source_path%InterfaceHandler.c %destination_path%InterfaceHandler.c