#ifndef MACROS_H
#define MACROS_H

#define getbit(p,m) ((p) & (m))
#define setbit(p,m) ((p) |= (m))
#define clearbit(p,m) ((p) &= ~(m))
#define flipbit(p,m) ((p) ^= (m))
#define writebit(c,p,m) (c ? setbit(p,m) : clearbit(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x)) 

// simulation mode
#ifdef AVRSIM
#define RETURN_IF_AVRSIM return
#else
#define RETURN_IF_AVRSIM 
#endif

#endif // #ifndef MACROS_H


