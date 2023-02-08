/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef BYTE_TAG_LIST_H
#define BYTE_TAG_LIST_H

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "ns3/type-id.h"
#include "tag-buffer.h"

namespace ns3 {

struct ByteTagListData;

/**
 * \ingroup packet
 *
 * \brief keep track of the byte tags stored in a packet.
 *
 * This class is mostly private to the Packet implementation and users
 * should never have to access it directly.
 *
 * \internal
 * The implementation of this class is a bit tricky so, there are a couple
 * of things to keep in mind here:
 *
 *   - It stores all tags in a single byte buffer: each tag is stored
 *     as 4 32bit integers (TypeId, tag data size, start, end) followed 
 *     by the tag data as generated by Tag::Serialize.
 *
 *   - The struct ByteTagListData structure which contains the tag byte buffer
 *     is shared and, thus, reference-counted. This data structure is unshared
 *     as-needed to emulate COW semantics.
 *
 *   - Each tag tags a unique set of bytes identified by the pair of offsets
 *     (start,end). These offsets are relative to the start of the packet
 *     Whenever the origin of the offset changes, the Packet adjusts all
 *     byte tags using ByteTagList::Adjust method.
 *
 *   - When packet is reduced in size, byte tags that span outside the packet
 *     boundaries remain in ByteTagList. It is not a problem as iterator fixes
 *     the boundaries before returning item. However, when packet is extending,
 *     it calls ByteTagList::AddAtStart or ByteTagList::AddAtEnd to cut byte
 *     tags that will otherwise cover new bytes.
 */
class ByteTagList
{
public:
  /**
   * \brief An iterator for iterating through a byte tag list
   *
   * An iterator for iterating through a byte tag list
   *
   */
  class Iterator
  {
  public:
    /**
     * \brief An item specifies an individual tag within a byte buffer
     *
     * An item specifies an individual tag within a byte buffer
     *
     */
    struct Item
    {
      TypeId tid; //!< type of the tag
      uint32_t size; //!< size of tag data
      int32_t start; //!< offset to the start of the tag from the virtual byte buffer
      int32_t end; //!< offset to the end of the tag from the virtual byte buffer
      TagBuffer buf; //!< the data for the tag as generated by Tag::Serialize

      /// Constructs an item with the given TagBuffer
      /// \param buf The Tag Buffer
      Item (TagBuffer buf);

    private:
      /// Friend class
      friend class ByteTagList;
      /// Friend class
      friend class ByteTagList::Iterator;
    };

    /**
     * \brief Used to determine if the iterator is at the end of the byteTagList
     *
     * \returns true if there are more Items in the list
     */
    bool HasNext (void) const;

    /**
     * \brief Returns the next Item from the ByteTagList
     *
     * \returns the next Item in the ByteTagList
     */
    struct ByteTagList::Iterator::Item Next (void);

    /**
     * \brief Returns the offset from the start of the virtual byte buffer to the ByteTagList
     *
     * \returns offset to the start of this ByteTagList
     */
    uint32_t GetOffsetStart (void) const;

  private:
    /// Friend class
    friend class ByteTagList;

    /**
     * \brief Constructor
     * \param start Starting tag
     * \param end End tag
     * \param offsetStart offset to the start of the tag from the virtual byte buffer
     * \param offsetEnd offset to the end of the tag from the virtual byte buffer
     * \param adjustment adjustment to byte tag offsets
     */
    Iterator (uint8_t *start, uint8_t *end, int32_t offsetStart, int32_t offsetEnd,
              int32_t adjustment);

    /**
     * \brief Prepare the iterator for the next tag
     */
    void PrepareForNext (void);
    uint8_t *m_current; //!< Current tag
    uint8_t *m_end; //!< End tag
    int32_t m_offsetStart; //!< Offset to the start of the tag from the virtual byte buffer
    int32_t m_offsetEnd; //!< Offset to the end of the tag from the virtual byte buffer
    int32_t m_adjustment; //!< Adjustment to byte tag offsets
    uint32_t m_nextTid; //!< TypeId of the next tag
    uint32_t m_nextSize; //!< Size of the next tag
    int32_t m_nextStart; //!< Start of the next tag
    int32_t m_nextEnd; //!< End of the next tag
  };

  ByteTagList ();

  /**
   * 
   * Copy constructor, copies the data and increases reference count
   *
   * \param o The ByteTagList to copy
   *
   */
  ByteTagList (const ByteTagList &o);

  /**
   *
   * Assignment operator, deallocates current data and assigns
   * value of passed in ByteTagList.  Also increases reference count
   *
   * \param o reference to the ByteTagList to copy
   * \returns reference to the assignee
   *
   */
  ByteTagList &operator= (const ByteTagList &o);
  ~ByteTagList ();

  /**
   * \param tid the typeid of the tag added
   * \param bufferSize the size of the tag when its serialization will 
   *        be completed. Typically, the return value of Tag::GetSerializedSize
   * \param start offset which uniquely identifies the first byte tagged by this tag.
   * \param end offset which uniquely identifies the last byte tagged by this tag.
   * \returns a buffer which can be used to write the tag data.
   *
   * 
   */
  TagBuffer Add (TypeId tid, uint32_t bufferSize, int32_t start, int32_t end);

  /**
   * \param o the other list of tags to aggregate.
   *
   * Aggregate the two lists of tags.
   */
  void Add (const ByteTagList &o);

  /**
   * 
   * Removes all of the tags from the ByteTagList
   */
  void RemoveAll (void);

  /**
   * \param offsetStart the offset which uniquely identifies the first data byte 
   *        present in the byte buffer associated to this ByteTagList.
   * \param offsetEnd the offset which uniquely identifies the last data byte 
   *        present in the byte buffer associated to this ByteTagList.
   * \returns an iterator
   *
   * The returned iterator will allow you to loop through the set of tags present
   * in this list: the boundaries of each tag as reported by their start and
   * end offsets will be included within the input offsetStart and offsetEnd.
   */
  ByteTagList::Iterator Begin (int32_t offsetStart, int32_t offsetEnd) const;

  /**
   * Adjust the offsets stored internally by the adjustment delta.
   *
   * \param adjustment value to change stored offsets by
   */
  inline void Adjust (int32_t adjustment);

  /**
   * Make sure that all offsets are smaller than appendOffset which represents
   * the location where new bytes have been added to the byte buffer.
   * 
   * \param appendOffset maximum offset value
   *
   */
  void AddAtEnd (int32_t appendOffset);
  /**
   * Make sure that all offsets are bigger than prependOffset which represents
   * the location where new bytes have been added to the byte buffer.
   *
   * \param prependOffset minimum offset value
   *
   */
  void AddAtStart (int32_t prependOffset);
  /**
   * Returns number of bytes required for packet serialization.
   *
   * \returns number of bytes required for packet serialization
   */
  uint32_t GetSerializedSize (void) const;
  /**
   * Serialize the tag list into a byte buffer.
   *
   * \param [in,out] buffer The byte buffer to which the tag list will be serialized
   * \param [in] maxSize Max The max size of the buffer for bounds checking
   *
   * \returns zero if complete tag list is not serialized
   */
  uint32_t Serialize (uint32_t *buffer, uint32_t maxSize) const;
  /**
   * Deserialize tag list from the provided buffer.
   *
   * \param [in] buffer The buffer to read from.
   * \param [in] size The number of bytes to deserialize.
   *
   * \returns zero if complete tag list is not deserialized
   */
  uint32_t Deserialize (const uint32_t *buffer, uint32_t size);

private:
  /**
   * \brief Returns an iterator pointing to the very first tag in this list.
   *
   * \returns an iterator
   */
  ByteTagList::Iterator BeginAll (void) const;

  /**
   * \brief Allocate the memory for the ByteTagListData
   * \param size the memory to allocate
   * \returns the ByteTagListData structure
   */
  struct ByteTagListData *Allocate (uint32_t size);

  /**
   * \brief Deallocates a ByteTagListData
   * \param data the ByteTagListData to deallocate
   */
  void Deallocate (struct ByteTagListData *data);

  int32_t m_minStart; //!< minimal start offset
  int32_t m_maxEnd; //!< maximal end offset
  int32_t m_adjustment; //!< adjustment to byte tag offsets
  uint32_t m_used; //!< the number of used bytes in the buffer
  struct ByteTagListData *m_data; //!< the ByteTagListData structure
};

void
ByteTagList::Adjust (int32_t adjustment)
{
  m_adjustment += adjustment;
}

} // namespace ns3

#endif /* BYTE_TAG_LIST_H */
