#define _sortkey_c_
#include <sortkey.h>

#include <errno.h>

/*
** {======================================================
** SortKey
** =======================================================
*/

uint64_t sortkey_encode(SortKey* key) {
  uint64_t viewId = ((uint64_t)key->viewId) << SORTKEY_VIEWID_SHIFT;
  uint64_t notTouch = ((uint64_t)key->notTouch) << SORTKEY_NOTTOUCH_SHIFT;
  if (!key->notTouch) {
    return viewId | notTouch;
  }
  uint64_t isDraw = ((uint64_t)key->isDraw) << SORTKEY_ISDRAW_SHIFT;
  if (key->isDraw) {
    uint64_t sortType = ((uint64_t)key->sortType) << SORTKEY_SORTTYPE_SHIFT;
    switch (key->sortType) {
      case ST_Program: {
        uint64_t blend = ((uint64_t)key->blend) << SORTKEY_BLEND_SHIFT0;
        uint64_t program = ((uint64_t)key->program) << SORTKEY_PROGRAM_SHIFT0;
        uint64_t depth = ((uint64_t)key->depth) << SORTKEY_DEPTH_SHIFT0;
        uint64_t sequence = ((uint64_t)key->sequence) << SORTKEY_SEQUENCE_SHIFT0;
        return viewId | notTouch | isDraw | sortType | blend | program | depth | sequence;
      } break;
      case ST_Depth: {
        uint64_t depth = ((uint64_t)key->depth) << SORTKEY_DEPTH_SHIFT1;
        uint64_t blend = ((uint64_t)key->blend) << SORTKEY_BLEND_SHIFT1;
        uint64_t program = ((uint64_t)key->program) << SORTKEY_PROGRAM_SHIFT1;
        uint64_t sequence = ((uint64_t)key->sequence) << SORTKEY_SEQUENCE_SHIFT1;
        return viewId | notTouch | isDraw | sortType | depth | blend | program | sequence;
      } break;
      case ST_Sequence: {
        uint64_t sequence = ((uint64_t)key->sequence) << SORTKEY_SEQUENCE_SHIFT2;
        uint64_t blend = ((uint64_t)key->blend) << SORTKEY_BLEND_SHIFT2;
        uint64_t program = ((uint64_t)key->program) << SORTKEY_PROGRAM_SHIFT2;
        return viewId | notTouch | isDraw | sortType | sequence | blend | program;
      } break;
    }
  } else {
    uint64_t sequence = ((uint64_t)key->sequence) << SORTKEY_COMPUTE_SEQUENCE_SHIFT;
    uint64_t program = ((uint64_t)key->program) << SORTKEY_COMPUTE_PROGRAM_SHIFT;
    return viewId | notTouch | isDraw | sequence | program;
  }
  return 0;
}

void sortkey_decode(SortKey* key, uint64_t value) {
  key->viewId = value >> SORTKEY_VIEWID_SHIFT;
  key->notTouch = value >> SORTKEY_NOTTOUCH_SHIFT;
  if (!key->notTouch) {
    return;
  }
  key->isDraw = value >> SORTKEY_ISDRAW_SHIFT;
  if (key->isDraw) {
    key->sortType = value >> SORTKEY_SORTTYPE_SHIFT;
    switch (key->sortType) {
      case ST_Program: {
        key->blend = value >> SORTKEY_BLEND_SHIFT0;
        key->program = value >> SORTKEY_PROGRAM_SHIFT0;
        key->depth = value >> SORTKEY_DEPTH_SHIFT0;
        key->sequence = value >> SORTKEY_SEQUENCE_SHIFT0;
      } break;
      case ST_Depth: {
        key->depth = value >> SORTKEY_DEPTH_SHIFT1;
        key->blend = value >> SORTKEY_BLEND_SHIFT1;
        key->program = value >> SORTKEY_PROGRAM_SHIFT1;
        key->sequence = value >> SORTKEY_SEQUENCE_SHIFT1;
      } break;
      case ST_Sequence: {
        key->sequence = value >> SORTKEY_SEQUENCE_SHIFT2;
        key->blend = value >> SORTKEY_BLEND_SHIFT2;
        key->program = value >> SORTKEY_PROGRAM_SHIFT2;
      } break;
    }
  } else {
    key->sequence = value >> SORTKEY_COMPUTE_SEQUENCE_SHIFT;
    key->program = value >> SORTKEY_COMPUTE_PROGRAM_SHIFT;
  }
}

static int _compare(const void* a, const void* b) {
  return *(uint64_t*)a - *(uint64_t*)b;
}
void sortUint64Array(uint64_t* values, uint32_t num) {
  // qsort and heapsort are not stable, mergesort is stable
  // if (mergesort((void*)values, num, sizeof(uint64_t), _compare) != 0) {
  //   int err = errno;
  //   printf_err("mergesort error: %d, %s\n", err, strerror(err));
  // }
  // heapsort and mergesort does not provide on Linux, so, using qsort
  qsort((void*)values, num, sizeof(uint64_t), _compare);
}

/* }====================================================== */
