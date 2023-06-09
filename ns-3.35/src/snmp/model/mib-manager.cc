/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * The GPLv2 License (GPLv2)
 *
 * Copyright (c) 2023 Rui Pedro C. Monteiro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http: //www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#include "mib-manager.h"
#include "ns3/timer.h"
#include "ns3/core-module.h"
#include <cstdio>
#include <fcntl.h>
#include <sys/file.h>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MibManager);

TypeId
MibManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MibManager").SetParent<Object> ().AddConstructor<MibManager> ();
  return tid;
}

MibManager::MibManager ()
{
}

MibManager::~MibManager ()
{
}

void
MibManager::InstallMib (Ptr<Mib> mib)
{
  m_mibs.push_back (mib);
}

void
MibManager::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void
MibManager::SetCommunity (std::string community)
{
  m_community = community;
}

void
MibManager::DumpMibs (Time interval, Time stop, std::string path)
{
  std::string nodeName = Names::FindName (m_node);
  std::string fullPath = SystemPath::Append (path, m_community) + ".snmprec";

  // First call to create files before starting SNMP sim
  DoDumpMibs (fullPath);

  Time i = Seconds (0.0);
  while (i <= stop)
    {
      Simulator::Schedule (i, &MibManager::DoDumpMibs, this, fullPath);
      i += interval;
    }
}

void
MibManager::DoDumpMibs (std::string path)
{
  // We use read mode here so we don't delete the contents
  // in case the SNMP sim is still reading
  FILE *fd = fopen (path.c_str (), "a");

  // This is an advisory lock, therefore it is ignored by the fstream
  // We just need to make sure it is respected by the SNMP sim
  flock (fileno (fd), LOCK_EX);

  Ptr<OutputStreamWrapper> streamWrapper = Create<OutputStreamWrapper> (path, std::ios::out);
  for (Ptr<Mib> mib : m_mibs)
    {
      mib->WriteToFile (streamWrapper);
    }

  // Release lock
  fclose (fd);
}

} // namespace ns3
