from geo.formats import simmob
import geo.helper


def serialize(rn :simmob.RoadNetwork, outFilePath :str):
  '''Serialize a Road Network to VISSIM's Abstract Network Model (ANM) format.'''
  print("Saving file:", outFilePath)
  out = open(outFilePath, 'w')

  rnInd = simmob.RNIndex(rn)

  out.write('<?xml  version = "1.0"  encoding = "UTF-8"?>\n')
  out.write('<ABSTRACTNETWORKMODEL\n')
  out.write('  VERSNO = "%s"\n' % '1.0')
  out.write('  FROMTIME = "%s"\n' % '08:00:00')  #We just make this up.
  out.write('  TOTIME = "%s"\n' % '12:00:00')    #Also made up.
  out.write('  NAME = "%s">\n' % outFilePath)
  out.write('  <NETWORK    LEFTHANDTRAFFIC = "%d">\n' % 1)
  __write_vehicle_types_and_classes(out)
  __write_nodes(out, rn, rnInd)
  __write_zones(out, rn)
  __write_link_types(out, rn)
  __write_links(out, rn)
  __write_stops_lines_signals(out, rn)
  out.write('  </NETWORK>\n')
  out.write('</ABSTRACTNETWORKMODEL>\n')

  #Done
  out.close()


def __write_vehicle_types_and_classes(out):
  #We might need to extrapolate here...
  out.write('    <VEHTYPES/>\n')
  out.write('    <VEHCLASSES/>\n')


def __write_zones(out, rn):
  #Currently nothing
  out.write('    <ZONES/>\n')


def __write_link_types(out, rn):
  #Currently we only need 1
  out.write('    <LINKTYPES>\n')
  out.write('      <LINKTYPE\n')
  out.write('        NO = "%s"\n' % 42)
  out.write('        NAME = "%s"\n' % 'Principal Arterial')
  out.write('        DRIVINGBEHAVIOR = "%s"\n' % 'Urban')
  out.write('      />\n')
  out.write('    </LINKTYPES>\n')


def __write_nodes(out, rn, rnInd):
  out.write('    <NODES>\n')
  for n in rn.nodes.values():
    #For now, only intersections
    if isinstance(n, simmob.Intersection):
      #Write the basic Node data:
      out.write('      <NODE\n')
      out.write('        NO = "%s"\n' % n.nodeId)
      out.write('        NAME = "%s"\n' % "")
      out.write('        XCOORD = "%s"\n' % n.pos.x)
      out.write('        YCOORD = "%s"\n' % n.pos.y)
      out.write('        CONTROLTYPE = "%s">\n' % "Unknown")

      #Lanes are just a list of which lanes actually meet at that junction.
      out.write('        <LANES>\n')
      _write_node_lanes(out, n, rnInd)
      out.write('        </LANES>\n')

      #Lane Turns connect these
      out.write('        <LANETURNS>\n')
      _write_node_lane_turns(out, n, rnInd)
      out.write('        </LANETURNS>\n')

      #Crosswalks aren't important.
      out.write('        <CROSSWALKS/>\n')

      #Done
      out.write('      </NODE>\n')
  out.write('    </NODES>\n')



def _write_node_lanes(out, n, rnInd):
  #Basically, for each segment, for each lane. But we propagate segments up to the "link" level.
  for seg in rnInd.segsAtNodes[n.nodeId]:
    for ln in seg.lanes:
      out.write('        <LANE\n')
      out.write('          LINKID = "%s"\n' % rnInd.segParents[seg.segId].linkId)
      out.write('          INDEX = "%s"\n' % ln.laneNumber)
      out.write('          POCKET = "%s"\n' % 'false')
      out.write('          POCKETLENGTH = "%s"\n' % 0.0000)
      out.write('          WIDTH = "%s"\n' % 3.5) #TODO: Actual lane width.
      out.write('        />\n')


def _write_node_lane_turns(out, n, rnInd):
  #These are relatively simple, and match our LaneConnector format. But we use "links" instead of segments, again.
  for seg in rnInd.segsAtNodes[n.nodeId]:
    for lcGrp in seg.lane_connectors.values():
      for lc in lcGrp:
        #We only care where lc.fromSeg matches the current segment (otherwise we're on the opposite end of the Segment).
        if lc.fromSegment.segId == seg.segId:
          out.write('        <LANETURN\n')
          out.write('          FROMLINKID = "%s"\n' % rnInd.segParents[lc.fromSegment.segId].linkId)
          out.write('          FROMLANEINDEX = "%s"\n' % lc.fromLane.laneNumber)
          out.write('          TOLINKID = "%s"\n' % rnInd.segParents[lc.toSegment.segId].linkId)
          out.write('          TOLANEINDEX = "%s"\n' % lc.toLane.laneNumber)
          out.write('        />\n')


def __write_links(out, rn):
  out.write('    <LINKS>\n')

  #We need a reverse lookup of Links. 
  rev_look = {}  #{(fromNodeID,toNodeID)=>Link}
  for lnk in rn.links.values():
    rev_look[(lnk.fromNode.nodeId,lnk.toNode.nodeId)] = lnk

  #Now, save each link:
  for lnk in rn.links.values():
      #We need the maximum number of lanes of the start/end segments, since we group segments as links.
      numLanes = max(len(lnk.segments[0].lanes),len(lnk.segments[-1].lanes))

      #Write it.
      out.write('      <LINK\n')
      out.write('        ID = "%s"\n' % lnk.linkId)
      out.write('        FROMNODENO = "%s"\n' % lnk.fromNode.nodeId)
      out.write('        TONODENO = "%s"\n' % lnk.toNode.nodeId)
      out.write('        LINKTYPENO = "%s"\n' % 42)
      out.write('        SPEED = "%s"\n' % 55) #Just a guess
      out.write('        NUMLANES = "%s"\n' % numLanes)

      #Reverse exists?
      revKey = (lnk.toNode.nodeId,lnk.fromNode.nodeId)
      if revKey in rev_look:
        out.write('        REVERSELINK = "%s"\n' % rev_look[revKey].linkId)

      out.write('      />\n')

  out.write('    </LINKS>\n')


def __write_stops_lines_signals(out, rn):
  #For now, nothing.
  out.write('    <PTSTOPS/>\n')
  out.write('    <PTLINES/>\n')
  out.write('    <SIGNALCONTROLS/>\n')

