# tools: move new check_mem here

import os
import gc # micropython garbage collection # use gc.mem_free() # use gc.collect()

DIAG = True # False # ___________________________________ global print disable switch / overwritten by console [D][enter]
DIAG = bool(os.getenv('DIAG')) # ______________________________ now get from settings.toml

def dp(line=" ", ende="\n"):
    if DIAG : print(line, end=ende)

def check_mem(info="",prints=True,coll=True) :
    if prints :
        dp("\n___ {:} check mem   : {:}".format( info, gc.mem_free()) )
    if coll :
        gc.collect()
    if ( prints and coll ) :
        dp("___ after clear : {:}".format( gc.mem_free()) )

# call:     check_mem(info = " JOBx after y",prints=True,coll=True)
