
GEN_CSRCS += $(notdir $(wildcard $(PRJ_DIR)/custom/*.c))

DEPPATH += --dep-path $(PRJ_DIR)/custom
VPATH += :$(PRJ_DIR)/custom

CFLAGS += "-I$(PRJ_DIR)/custom"
CFLAGS += -DREPPO_UI_HAS_WORD_AI_SCREENS=0 -DREPPO_UI_LARGE_FONT=lv_font_SourceHanSansSC_Medium_20
