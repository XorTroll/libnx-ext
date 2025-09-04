#include <switch-ipcext.h>
#include <switch.h>
#include <stdio.h>

NsApplicationControlData data = {};
    
void failrc(const char *err, Result rc) {
    printf("Failure: %s -> 0x%X / %04d-%04d\n", err, R_VALUE(rc), 2000 + R_MODULE(rc), R_DESCRIPTION(rc));
    consoleUpdate(NULL);
    while(appletMainLoop());
}

#define RC_ASSERT(expr) ({ \
    const auto _tmp = (expr); \
    if(R_FAILED(_tmp)) { \
        failrc(#expr, _tmp); \
    } \
})

PadState pad;

int main(int argc, char* argv[]) {
    consoleInit(NULL);
    printf("ncm-ext control data example\n\n");
    consoleUpdate(NULL);

    RC_ASSERT(ncmInitialize());

    padInitializeDefault(&pad);
    padConfigureInput(8, HidNpadStyleSet_NpadStandard);

    u64 test_app_id = 0x01009B90006DC000; /* Super Mario Maker 2 */
    size_t read_size;
    SetLanguage out_lang;
    RC_ASSERT(ncmextReadApplicationControlDataManual(SetLanguage_ES, test_app_id, &data, sizeof(data), &read_size, &out_lang));

    printf("NACP version (%016lX): %s\n", test_app_id, data.nacp.display_version);

    while(appletMainLoop()) {
        padUpdate(&pad);
        if(padGetButtonsDown(&pad) & HidNpadButton_Plus) {
            break;
        }

        consoleUpdate(NULL);
    }

    printf("Exiting...\n");
    ncmExit();
    consoleExit(NULL);
    return 0;
}
