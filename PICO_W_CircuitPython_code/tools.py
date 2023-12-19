# tools: move new check_mem here

import os
import gc # micropython garbage collection # use gc.mem_free() # use gc.collect()
print(f"\nFREE MEM report from tools.py after imports\n+ import gc {gc.mem_free()}")

DIAG = True # False # ___________________________________ global print disable switch
DIAG = bool(os.getenv('DIAG')) # ________________________ now get from settings.toml

DIAGM = True # False # __________________________________ global memory diagnostic
DIAGM = bool(os.getenv('DIAGM')) # ______________________ now get from settings.toml

if DIAGM : print("+++ DIAGM enabled, see file 'settings.toml'")

def dp(line=" ", ende="\n"):
    if DIAG : print(line, end=ende)

def check_mem(info="",prints=True,coll=True) :
    if prints :
        freeold=gc.mem_free()
    if coll :
        gc.collect()
        if ( prints ) :
            print(f"\n___ {info} check mem : {freeold} after clear : {gc.mem_free()} " )
    else :
        if ( prints ) :
            print(f"\n___ {info} check mem : {freeold} " )

# call:     check_mem(info = " JOBx after y",prints=True,coll=True)
