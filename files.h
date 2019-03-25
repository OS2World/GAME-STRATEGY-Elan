
#include <stdio.h>
#include <string.h>

#define SAVEFILE_NAME       "elan.sav"
#define DUMPFILE_NAME       "elan.map"

#define NULL_REF    -1
#define HEADER_LEN  256
#define HEADER_TAG  "@" "Elan" " " VER " " SYSTEM "@"

/* Protos */
void save_game(void);
void load_game(void);
error_class save_state(void);
error_class load_state(void);
size_t write_device_ref(device *plant, FILE *fp);
device *read_device_ref(FILE *fp);
size_t write_channel_ref(channel *duct, FILE *fp);
channel *read_channel_ref(FILE *fp);
size_t write_mobile_ref(mobile *turtle, FILE *fp);
mobile *read_mobile_ref(FILE *fp);
size_t write_cell_ref(cell *loc, FILE *fp);
cell *read_cell_ref(FILE *fp);
error_class dump_map(void);

