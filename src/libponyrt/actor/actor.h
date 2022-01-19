#ifndef actor_actor_h
#define actor_actor_h

#include "messageq.h"
#include "../gc/gc.h"
#include "../mem/heap.h"
#include "../pony.h"
#include <stdint.h>
#include <stdbool.h>
#ifndef __cplusplus
#  include <stdalign.h>
#endif
#include <platform.h>

PONY_EXTERN_C_BEGIN

// Reminder: When adding new types, please also update
// examples/dtrace/telemetry.d

#define ACTORMSG_APPLICATION_START (UINT32_MAX - 11)
#define ACTORMSG_CHECKBLOCKED (UINT32_MAX - 10)
#define ACTORMSG_DESTROYED (UINT32_MAX - 9)
#define ACTORMSG_ISBLOCKED (UINT32_MAX - 7)
#define ACTORMSG_BLOCK (UINT32_MAX - 6)
#define ACTORMSG_UNBLOCK (UINT32_MAX - 5)
#define ACTORMSG_ACQUIRE (UINT32_MAX - 4)
#define ACTORMSG_RELEASE (UINT32_MAX - 3)
#define ACTORMSG_CONF (UINT32_MAX - 2)
#define ACTORMSG_ACK (UINT32_MAX - 1)

typedef struct pony_actor_t
{
  pony_type_t* type;
  messageq_t q;
  PONY_ATOMIC(size_t) muted;
  PONY_ATOMIC(uint8_t) flags;
  PONY_ATOMIC(uint8_t) is_muted;

  // keep things accessed by other actors on a separate cache line
  alignas(64) heap_t heap; // 52/104 bytes
  gc_t gc; // 48/88 bytes
} pony_actor_t;

/** Padding for actor types.
 *
 * 56 bytes: initial header, not including the type descriptor
 * 52/104 bytes: heap
 * 48/88 bytes: gc
 * 28/0 bytes: padding to 64 bytes, ignored
 */
#if INTPTR_MAX == INT64_MAX
#ifdef USE_MEMTRACK
#  define PONY_ACTOR_PAD_SIZE 280
#else
#  define PONY_ACTOR_PAD_SIZE 248
#endif
#elif INTPTR_MAX == INT32_MAX
#ifdef USE_MEMTRACK
#  define PONY_ACTOR_PAD_SIZE 176
#else
#  define PONY_ACTOR_PAD_SIZE 160
#endif
#endif

typedef struct pony_actor_pad_t
{
  pony_type_t* type;
  char pad[PONY_ACTOR_PAD_SIZE];
} pony_actor_pad_t;

enum
{
  FLAG_BLOCKED = 1 << 0,
  FLAG_BLOCKED_SENT = 1 << 1,
  FLAG_SYSTEM = 1 << 2,
  FLAG_UNSCHEDULED = 1 << 3,
  FLAG_PENDINGDESTROY = 1 << 4,
  FLAG_OVERLOADED = 1 << 5,
  FLAG_UNDER_PRESSURE = 1 << 6,
  FLAG_CD_CONTACTED = 1 << 7,
};

bool has_flag(pony_actor_t* actor, uint8_t flag);

bool ponyint_actor_run(pony_ctx_t* ctx, pony_actor_t* actor, bool polling);

void ponyint_actor_destroy(pony_actor_t* actor);

gc_t* ponyint_actor_gc(pony_actor_t* actor);

heap_t* ponyint_actor_heap(pony_actor_t* actor);

bool ponyint_actor_pendingdestroy(pony_actor_t* actor);

void ponyint_actor_setpendingdestroy(pony_actor_t* actor);

void ponyint_actor_final(pony_ctx_t* ctx, pony_actor_t* actor);

void ponyint_actor_sendrelease(pony_ctx_t* ctx, pony_actor_t* actor);

void ponyint_actor_setsystem(pony_actor_t* actor);

void ponyint_actor_setnoblock(bool state);

bool ponyint_actor_getnoblock();

void ponyint_actor_setoverloaded(pony_actor_t* actor);

void ponyint_actor_unsetoverloaded(pony_actor_t* actor);

PONY_API void pony_apply_backpressure();

PONY_API void pony_release_backpressure();

void ponyint_maybe_mute(pony_ctx_t* ctx, pony_actor_t* to);

bool ponyint_triggers_muting(pony_actor_t* actor);

bool ponyint_is_muted(pony_actor_t* actor);

void ponyint_unmute_actor(pony_actor_t* actor);

void ponyint_mute_actor(pony_actor_t* actor);

PONY_API void ponyint_destroy(pony_ctx_t* ctx, pony_actor_t* actor);

#ifdef USE_MEMTRACK
size_t ponyint_actor_mem_size(pony_actor_t* actor);

size_t ponyint_actor_alloc_size(pony_actor_t* actor);

size_t ponyint_actor_total_mem_size(pony_actor_t* actor);

size_t ponyint_actor_total_alloc_size(pony_actor_t* actor);
#endif

PONY_EXTERN_C_END

#endif
