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

#include "mib-logger.h"
#include "ns3/abort.h"
#include "ns3/mib-manager.h"
#include "ns3/core-module.h"
#include "ns3/system-path.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

namespace ns3 {

static GlobalValue g_LogEnabled = GlobalValue ("MibLoggerEnabled", "MibLogger enbaled",
                                               BooleanValue (false), MakeBooleanChecker ());

TypeId
MibLogger::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MibLogger");
  return tid;
}

MibLogger::MibLogger (std::string path)
{
  m_path = path;
  m_simPid = 0;
}

MibLogger::~MibLogger ()
{
}

void
MibLogger::EnableDump (Time interval, Time stop, Ptr<Node> node)
{
  Ptr<MibManager> manager = node->GetObject<MibManager> ();
  if (manager)
    {
      std::string nodeName = Names::FindName (node);
      std::string path = SystemPath::Append (m_path, nodeName);

      // Create directory
      SystemPath::MakeDirectories (path);
      m_dataDirs.push_back (path);

      manager->DumpMibs (interval, stop, path);
    }
}

void
MibLogger::StartSnmpSim ()
{
  uint32_t port = 1024;

  std::vector<std::string> args;
  std::vector<const char *> argv;

  for (std::string dataDir : m_dataDirs)
    {
      args.push_back ("--v3-engine-id=auto");
      args.push_back ("--data-dir=" + dataDir);
      args.push_back ("--agent-udpv4-endpoint=127.0.0.1:" + std::to_string (port));
      port++;
    }

  for (const auto &s : args)
    {
      argv.push_back (s.data ());
    }
  argv.push_back (NULL);
  argv.shrink_to_fit ();

  pid_t pid = fork ();
  NS_ABORT_MSG_IF (pid == -1, "fork() failed");
  if (pid == 0) // child
    {
      // Redirect verbose output
      close (STDOUT_FILENO);
      close (STDERR_FILENO);

      std::string totalPath = SystemPath::Append (m_path, "snmpsim.output");
      int fd = open (totalPath.c_str (), O_WRONLY | O_CREAT, 0666);

      if (fd < 0)
        NS_ABORT_MSG ("failed opening snmpsim.output file");

      dup2 (fd, STDOUT_FILENO);
      dup2 (fd, STDERR_FILENO);

      int st = execvp ("snmpsim-command-responder", const_cast<char *const *> (argv.data ()));
      NS_ABORT_MSG ("execl failed with status " << st);
    }
  else // parent
    {
      m_simPid = pid;
    }
}

void
MibLogger::StopSnmpSim ()
{
  // NOTE: Probably not ideal
  if (m_simPid != 0)
    {
      kill (m_simPid, SIGKILL);
    }
}

void
MibLogger::EnableDump (Time interval, Time stop, NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); i++)
    {
      EnableDump (interval, stop, *i);
    }
}

void
MibLogger::EnableDumpAll (Time interval, Time stop)
{
  EnableDump (interval, stop, NodeContainer::GetGlobal ());
}

bool
MibLogger::IsEnabled ()
{
  BooleanValue val;
  g_LogEnabled.GetValue (val);
  return val.Get ();
}

} // namespace ns3
