#ifndef UISP_H
#define UISP_H


#if defined(CONFIG_RUN_BUTTON_ON_START) || defined(CONFIG_RUN_BUTTON_ON_LOOP)
#define initRunButton() CONFIG_RUN_BUTTON_PORT |= (1<<CONFIG_RUN_BUTTON_BIT);
#define checkRunButton() (CONFIG_RUN_BUTTON_PIN & (1<<CONFIG_RUN_BUTTON_BIT))
#else
#define initRunButton()
#define checkRunButton() (0)
#endif

/* Properly handle devices with bigger flash */
#if (((FLASHEND) > 0xffff) || ((E2END) > 0xffff))
typedef uint32_t addr_t;
#define do_memcpy(d,s,l) memcpy_PF(d,s,l)
#else
typedef uint16_t addr_t;
#define do_memcpy(d,s,l) memcpy_P(d,s,l)
#endif

#ifdef UHID_TINY
#	define IOBUFLEN 2
#else
#	define IOBUFLEN 64
#endif


/* The version 1 spec structures */
#define UISP_PART_NAME_LEN  8
#define UISP_INFO_LEN   (sizeof(struct deviceInfo) + 2 * sizeof(struct partInfo))

struct partInfo {
	uint16_t      pageSize;
	uint32_t      size;
	uint8_t       ioSize;
	uint8_t       name[UISP_PART_NAME_LEN];
}  __attribute__((packed));

struct deviceInfo {
	uint8_t       version;
	uint8_t       numParts;
	uint16_t       cpuFreq;
	struct partInfo parts[];
} __attribute__((packed));


#endif
