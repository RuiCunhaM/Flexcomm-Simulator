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

#include "energy-api.h"
#include "fcntl.h"
#include "signal.h"

namespace ns3 {

using namespace std;

NS_OBJECT_ENSURE_REGISTERED (EnergyAPI);

map<string, vector<float>> EnergyAPI::m_flexes = map<string, vector<float>> ();
map<string, vector<float>> EnergyAPI::m_estimates = map<string, vector<float>> ();
pid_t EnergyAPI::m_pid = 0;

TypeId
EnergyAPI::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EnergyAPI").SetParent<Object> ().AddConstructor<EnergyAPI> ();
  return tid;
}

EnergyAPI::EnergyAPI ()
{
}

EnergyAPI::~EnergyAPI ()
{
  StopExternalServer ();
}

vector<float>
EnergyAPI::GetFlexArray (string id)
{
  // TODO: Check if id exists
  return m_flexes[id];
}

vector<float>
EnergyAPI::GetEstimateArray (string id)
{
  // TODO: Check if id exists
  return m_estimates[id];
}

void
EnergyAPI::AddFlexArray (string id, vector<float> arr)
{
  m_flexes[id] = arr;
}

void
EnergyAPI::AddEstimateArray (string id, vector<float> arr)
{
  m_estimates[id] = arr;
}

void
EnergyAPI::StartExternalServer (string topo)
{
  if (m_pid != 0)
    NS_ABORT_MSG ("External EnergyAPI server already running");

  pid_t pid = fork ();
  NS_ABORT_MSG_IF (pid == -1, "EnergyAPI: fork() failed");
  if (pid == 0)
    {
      std::string outPath = SystemPath::Append (topo, "energyAPI.out");
      int outfd = open (outPath.c_str (), O_WRONLY | O_CREAT, 0666);
      close (STDOUT_FILENO);
      close (STDERR_FILENO);
      dup2 (outfd, STDOUT_FILENO);
      dup2 (outfd, STDERR_FILENO);

      list<string> paths = ns3::SystemPath::Split (ns3::SystemPath::FindSelfDirectory ());
      paths.erase (std::prev (paths.end (), 2), paths.end ());
      paths.insert (paths.end (), {"utils", "energy-api-server.py"});
      std::string executable = ns3::SystemPath::Join (paths.begin (), paths.end ());

      list<string> argsPath = {"..", "topologies", topo};
      std::string args = SystemPath::Join (argsPath.begin (), argsPath.end ());

      int st = execl (executable.c_str (), executable.c_str (), args.c_str (), NULL);
      NS_ABORT_MSG ("EnergyAPI: execl failed with status " << st);
    }
  else
    {
      m_pid = pid;
    }
}

void
EnergyAPI::StopExternalServer ()
{
  if (m_pid > 0)
    {
      kill (m_pid, SIGKILL);
      m_pid = 0;
    }
}

} // namespace ns3
