/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 CNRS, France.
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
 * Author: Anne-Cecile Orgerie <anne-cecile.orgerie@irisa.fr>
 */

#include "agenda.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include <vector>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Agenda);

TypeId
Agenda::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Agenda").SetParent<Object> ().AddConstructor<Agenda> ();
  return tid;
}

Agenda::Agenda ()
{
}

Agenda::~Agenda ()
{
}

void
Agenda::SetNetdev (Ptr<NetDevice> netdev)
{
  NS_ASSERT (netdev != NULL);
  m_netdev = netdev;
}

Ptr<NetDevice>
Agenda::GetNetdev (void) const
{
  return m_netdev;
}

void
Agenda::AddEvent (double start, double stop)
{
  if (m_agenda.size () == 0)
    {
      m_agenda.push_back (start);
      m_agenda.push_back (stop);
    }
  else
    {
      uint32_t i = 0;
      uint32_t j = 0;
      while ((start > m_agenda[i]) && (i < m_agenda.size ()))
        {
          i++;
        }
      while ((stop > m_agenda[j]) && (j < m_agenda.size ()))
        {
          j++;
        }
      // if start is after all the agenda's dates
      if (i == m_agenda.size ())
        {
          m_agenda.push_back (start);
          m_agenda.push_back (stop);
        }
      // if stop is before all the agenda's dates
      else if (j == 0)
        {
          // if stop is equal to the first date
          if (m_agenda[0] == stop)
            {
              m_agenda[0] = start;
            }
          // otherwise
          else
            {
              std::vector<double>::iterator it;
              it = m_agenda.begin ();
              m_agenda.insert (it, stop);
              it = m_agenda.begin ();
              m_agenda.insert (it, start);
            }
        }
      // if there is no date between start and stop
      else if (i == j)
        {
          // if i is a starting date
          if (i % 2 == 0)
            {
              std::vector<double>::iterator it;
              it = m_agenda.begin ();
              m_agenda.insert (it + i, start);
              // if stop is already an agenda's date
              if (stop == m_agenda[j])
                {
                  it = m_agenda.begin ();
                  m_agenda.erase (it + i + 1); //position i+1
                }
              // if stop is not an agenda's date
              else
                {
                  it = m_agenda.begin ();
                  m_agenda.insert (it + i + 1, stop);
                }
            }
          // if i is an ending date, nothing to do
        }
      else // i!= j
        {
          std::vector<double>::iterator it;
          uint32_t size = m_agenda.size ();
          double agj;
          uint32_t z = 0;
          if (j != size)
            {
              agj = m_agenda[j];
            }
          it = m_agenda.begin ();
          m_agenda.erase (it + i, it + j); // erase [i, j[
          // if i is a starting date
          if (i % 2 == 0)
            {
              it = m_agenda.begin ();
              m_agenda.insert (it + i, start);
              z++;
            }
          // else -> nothing to do
          // if j is a starting date
          if (j % 2 == 0)
            {
              // if stop is already an agenda's date
              if (j == size)
                {
                  m_agenda.push_back (stop);
                }
              else if (stop == agj)
                {
                  it = m_agenda.begin ();
                  m_agenda.erase (it + i + z); //position i
                }
              // if stop is not an agenda's date
              else
                {
                  it = m_agenda.begin ();
                  m_agenda.insert (it + i + z, stop);
                }
            }
          // else -> nothing to do
        }
    }
}

std::vector<double>
Agenda::GetTab () const
{
  return m_agenda;
}

} // namespace ns3
