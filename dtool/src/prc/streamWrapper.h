/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file streamWrapper.h
 * @author drose
 * @date 2008-11-11
 */

#ifndef STREAMWRAPPER_H
#define STREAMWRAPPER_H

#include "dtoolbase.h"
#include "mutexImpl.h"

/**
 * The base class for both IStreamWrapper and OStreamWrapper, this provides
 * the common locking interface.
 */
class EXPCL_DTOOL_PRC StreamWrapperBase {
protected:
  INLINE StreamWrapperBase();

private:
  INLINE StreamWrapperBase(const StreamWrapperBase &copy) DELETED;

PUBLISHED:
  INLINE void acquire();
  INLINE void release();

private:
  MutexImpl _lock;
#ifdef SIMPLE_THREADS
  // In the SIMPLE_THREADS case, we need to use a bool flag, because MutexImpl
  // defines to nothing in this case--but we still need to achieve a form of
  // locking, since IO operations can cause the thread to swap without
  // warning.
  bool _lock_flag;
#endif
};

/**
 * This class provides a locking wrapper around an arbitrary istream pointer.
 * A thread may use this class to perform an atomic seek/read/gcount
 * operation.
 */
class EXPCL_DTOOL_PRC IStreamWrapper : virtual public StreamWrapperBase {
public:
  INLINE IStreamWrapper(istream *stream, bool owns_pointer);
PUBLISHED:
  INLINE explicit IStreamWrapper(istream &stream);
  ~IStreamWrapper();

  INLINE istream *get_istream() const;
  MAKE_PROPERTY(istream, get_istream);

public:
  void read(char *buffer, streamsize num_bytes);
  void read(char *buffer, streamsize num_bytes, streamsize &read_bytes);
  void read(char *buffer, streamsize num_bytes, streamsize &read_bytes, bool &eof);
  void seek_read(streamsize pos, char *buffer, streamsize num_bytes, streamsize &read_bytes, bool &eof);
  INLINE int get();
  streamsize seek_gpos_eof();

private:
  istream *_istream;
  bool _owns_pointer;
};

/**
 * This class provides a locking wrapper around an arbitrary ostream pointer.
 * A thread may use this class to perform an atomic seek/write operation.
 */
class EXPCL_DTOOL_PRC OStreamWrapper : virtual public StreamWrapperBase {
public:
  INLINE OStreamWrapper(ostream *stream, bool owns_pointer, bool stringstream_hack = false);
PUBLISHED:
  INLINE explicit OStreamWrapper(ostream &stream);
  ~OStreamWrapper();

  INLINE ostream *get_ostream() const;
  MAKE_PROPERTY(ostream, get_ostream);

public:
  void write(const char *buffer, streamsize num_bytes);
  void write(const char *buffer, streamsize num_bytes, bool &fail);
  void seek_write(streamsize pos, const char *buffer, streamsize num_bytes, bool &fail);
  void seek_eof_write(const char *buffer, streamsize num_bytes, bool &fail);
  INLINE bool put(char c);
  streamsize seek_ppos_eof();

private:
  ostream *_ostream;
  bool _owns_pointer;

  // This flag is necessary to work around a weird quirk in the MSVS C++
  // runtime library: an empty stringstream cannot successfully seekp(0),
  // until some data has been written to the stream.  When this flag is set
  // true, we know we have a possibly-empty stringstream, so we allow seekp(0)
  // to fail silently, knowing that there's no harm in this case.
#ifdef WIN32_VC
  bool _stringstream_hack;
#endif
};

/**
 * This class provides a locking wrapper around a combination ostream/istream
 * pointer.
 */
class EXPCL_DTOOL_PRC StreamWrapper : public IStreamWrapper, public OStreamWrapper {
public:
  INLINE StreamWrapper(iostream *stream, bool owns_pointer, bool stringstream_hack = false);
PUBLISHED:
  INLINE explicit StreamWrapper(iostream &stream);
  ~StreamWrapper();

  INLINE iostream *get_iostream() const;
  MAKE_PROPERTY(iostream, get_iostream);

private:
  iostream *_iostream;
  bool _owns_pointer;
};

#include "streamWrapper.I"

#endif
