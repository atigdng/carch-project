# ==============================================================================
# Makefile: Automated gem5 Build + Simulation Workflow
#
# This file compiles C programs, converts them to assembly, builds RISC-V
# binaries, and runs them on multiple gem5 CPU models (O3CPU + MinorCPU).
# It can also generate traces and pipeline-view dumps.
# ==============================================================================

# --------------------- Directories ---------------------
G5_ROOT      = gem5
SRC_DIR      = codes
ASM_OUT      = asm
BIN_OUT      = bin
RESULTS_DIR  = out

# --------------------- Toolchain -----------------------
CC      = riscv64-linux-gnu-gcc
CFLAGS  = -O3 --static

G5_EXEC     = $(G5_ROOT)/build/RISCV/gem5.opt
G5_SE_SCRIPT= $(G5_ROOT)/configs/deprecated/example/se.py
PIPEVIEW    = $(G5_ROOT)/util/o3-pipeview.py

# --------------------- CPU Configurations ----------------
O3_CFG    = --cpu-type=O3CPU   --caches --l2cache
MINOR_CFG = --cpu-type=MinorCPU --caches --l2cache

TRACE_O3    = --debug-flags=O3CPU,Exec
TRACE_MINOR = --debug-flags=MinorCPU,Exec

PV_FLAGS    = --debug-flags=O3PipeView
CYCLE_SCALE = 500

# --------------------- File Enumeration ------------------
SRCS      = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(patsubst $(SRC_DIR)/%.c, $(ASM_OUT)/%.s, $(SRCS))
BIN_FILES = $(patsubst $(ASM_OUT)/%.s, $(BIN_OUT)/%, $(ASM_FILES))

O3_STATS       = $(patsubst $(BIN_OUT)/%, $(RESULTS_DIR)/%_o3/stats.txt, $(BIN_FILES))
MINOR_STATS    = $(patsubst $(BIN_OUT)/%, $(RESULTS_DIR)/%_minor/stats.txt, $(BIN_FILES))

O3_TRACE_OUT   = $(patsubst $(BIN_OUT)/%, $(RESULTS_DIR)/%_o3_tr/trace.done, $(BIN_FILES))
MIN_TRACE_OUT  = $(patsubst $(BIN_OUT)/%, $(RESULTS_DIR)/%_minor_tr/trace.done, $(BIN_FILES))

PIPEVIEW_FILES = $(patsubst $(BIN_OUT)/%, $(RESULTS_DIR)/%_pv/pipeview.out, $(BIN_FILES))

# --------------------- Main Targets ----------------------
all: o3

o3: $(O3_STATS)
minor: $(MINOR_STATS)
both: o3 minor

trace: $(O3_TRACE_OUT)
trace_minor: $(MIN_TRACE_OUT)
trace_all: trace trace_minor

pipeview: $(PIPEVIEW_FILES)

clean:
	@echo "Cleaning all generated output..."
	rm -rf $(ASM_OUT) $(BIN_OUT) $(RESULTS_DIR)


# --------------------- Build Steps -----------------------

# Step 1: C → Assembly
$(ASM_OUT)/%.s: $(SRC_DIR)/%.c
	@echo "[1] Generating assembly for $<"
	@mkdir -p $(ASM_OUT)
	$(CC) $(CFLAGS) -S -o $@ $<

# Step 2: Assembly → Binary
$(BIN_OUT)/%: $(ASM_OUT)/%.s
	@echo "[2] Building binary for $<"
	@mkdir -p $(BIN_OUT)
	$(CC) $(CFLAGS) -o $@ $<

# Step 3A: Run O3CPU statistics
$(RESULTS_DIR)/%_o3/stats.txt: $(BIN_OUT)/%
	@echo "[3] O3CPU simulation: $<"
	@mkdir -p $(@D)
	$(G5_EXEC) --outdir=$(@D) --listener-mode=off \
		$(G5_SE_SCRIPT) $(O3_CFG) -c $<

# Step 3B: Run MinorCPU statistics
$(RESULTS_DIR)/%_minor/stats.txt: $(BIN_OUT)/%
	@echo "[3] MinorCPU simulation: $<"
	@mkdir -p $(@D)
	$(G5_EXEC) --outdir=$(@D) --listener-mode=off \
		$(G5_SE_SCRIPT) $(MINOR_CFG) -c $<

# Step 4A: O3CPU Trace
$(RESULTS_DIR)/%_o3_tr/trace.done: $(BIN_OUT)/%
	@echo "Trace (O3CPU) for $<"
	@mkdir -p $(@D)
	$(G5_EXEC) --outdir=$(@D) $(TRACE_O3) --listener-mode=off \
		$(G5_SE_SCRIPT) $(O3_CFG) -c $< \
		> $(@D)/gem5_trace.log 2>&1
	@touch $@

# Step 4B: MinorCPU Trace
$(RESULTS_DIR)/%_minor_tr/trace.done: $(BIN_OUT)/%
	@echo "Trace (MinorCPU) for $<"
	@mkdir -p $(@D)
	$(G5_EXEC) --outdir=$(@D) $(TRACE_MINOR) --listener-mode=off \
		$(G5_SE_SCRIPT) $(MINOR_CFG) -c $< \
		> $(@D)/gem5_trace.log 2>&1
	@touch $@

# Step 5: O3PipeView
$(RESULTS_DIR)/%_pv/pipeview.out: $(BIN_OUT)/%
	@echo "Pipeview generation for $<"
	@mkdir -p $(@D)
	$(G5_EXEC) --outdir=$(@D) --debug-file=trace.raw \
		$(PV_FLAGS) --listener-mode=off \
		$(G5_SE_SCRIPT) $(O3_CFG) -c $<

	$(PIPEVIEW) -c $(CYCLE_SCALE) -o $@ --color \
		$(@D)/trace.raw

# --------------------- Make Flags ------------------------
.SECONDARY:
.PHONY: all clean o3 minor both trace trace_minor trace_all pipeview