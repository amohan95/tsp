#include "tsplib/tsp.h"

#include <fstream>

#include "tsplib/coord/distance_calc_factory.h"

using namespace std;

TSP::TSP() {
}

TSP::~TSP() {
  if (node_coords_) {
    for (int i = 0; i < dimension_; ++i) {
      delete node_coords_[i];
    }
    delete [] node_coords_;
  }
  if (graph_) {
    delete graph_;
  }
}

bool TSP::BuildGraph() {
  if (graph()) {
    return true;
  }
  if (edge_weight_type() == kExplicit) {
    return false;
  } else {
    if (node_coords_ == NULL) {
      return false;
    }
    graph_ = new Graph(dimension());
    DistanceCalc* distance_calc = DistanceCalcFactory::GetDistanceCalc(
      edge_weight_type()
    );
    for (int i = 0; i < dimension(); ++i) {
      for (int j = i + 1; j < dimension(); ++j) {
        if (!distance_calc->VerifyCoordDimensions(node_coords_[i], node_coords_[j])) {
          return false;
        }
        int distance = distance_calc->Distance(node_coords_[i], node_coords_[j]);
        graph_->SetEdgeWeight(i, j, distance);
      }
    }
    delete distance_calc;
    return true;
  }
}

bool TSP::Parse(string file_name) {
  ifstream is(file_name);
  string entry_key;
  is >> entry_key;
  while (entry_key != "EOF") {
    if (entry_key == "NAME:") {
      is >> name_;
    } else if (entry_key == "TYPE:") {
      tsp_type_ = static_cast<TSPType>(
        ParseEnumEntry(is, kTSPTypeValues, kNumTSPTypes)
      );
      if (tsp_type_ == -1) {
        return false;
      }
    } else if (entry_key == "COMMENT:") {
      getline(is, comment_);
    } else if (entry_key == "DIMENSION:") {
      is >> dimension_;
    } else if (entry_key == "CAPACITY:") {
      is >> capacity_;
    } else if (entry_key == "EDGE_WEIGHT_TYPE:") {
      edge_weight_type_ = static_cast<EdgeWeightType>(
        TSP::ParseEnumEntry(is, kEdgeWeightTypeValues, kNumEdgeWeightTypes)
      );
      if (edge_weight_type_ == -1) {
        return false;
      }
    } else if (entry_key == "EDGE_WEIGHT_FORMAT:") {
      edge_weight_format_ = static_cast<EdgeWeightFormat>(
        TSP::ParseEnumEntry(is, kEdgeWeightFormatValues, kNumEdgeWeightFormats)
      );
      if (edge_weight_format_ == -1) {
        return false;
      }
    } else if (entry_key == "EDGE_DATA_FORMAT:") {
      edge_data_format_ = static_cast<EdgeDataFormat>(
        TSP::ParseEnumEntry(is, kEdgeDataFormatValues, kNumEdgeDataFormats)
      );
      if (edge_data_format_ == -1) {
        return false;
      }
    } else if (entry_key == "NODE_COORD_TYPE:") {
      node_coord_type_ = static_cast<NodeCoordType>(
        TSP::ParseEnumEntry(is, kNodeCoordTypeValues, kNumNodeCoordTypes)
      );
      if (node_coord_type_ == -1) {
        return false;
      }
    } else if (entry_key == "DISPLAY_DATA_TYPE:") {
      display_data_type_ = static_cast<DisplayDataType>(
        TSP::ParseEnumEntry(is, kDisplayDataTypeValues, kNumDisplayDataTypes)
      );
      if (display_data_type_ == -1) {
        return false;
      }
    } else if (entry_key == "NODE_COORD_SECTION") {
      if (!ParseNodeCoordSection(is)) {
        return false;
      }
    }
    is >> entry_key;
  }
  is.close();
  return true;
}

int TSP::ParseEnumEntry(istream& is, const string values[], const int num_values) {
  string value;
  is >> value;
  for (int i = 0; i < num_values; ++i) {
    if (value == values[i]) {
      return i;
    }
  }
  return -1;
}

bool TSP::ParseNodeCoordSection(istream& is) {
  if (dimension() == -1 || node_coord_type() == kNoCoords) {
    return false;
  }
  int i = 0;
  double x, y;
  node_coords_ = new Coord*[dimension()];
  while (i < dimension()) {
    is >> i >> x >> y;
    if (!is.good()) {
      return false;
    }
    Coord* coord = NULL;
    if (node_coord_type() == kThreeDCoords) {
      double z;
      is >> z;
      coord = new Coord(3);
      coord->SetCoordinate(0, x);
      coord->SetCoordinate(1, y);
      coord->SetCoordinate(2, z);
    } else {
      coord = new Coord(2);
      coord->SetCoordinate(0, x);
      coord->SetCoordinate(1, y);
    }
    node_coords_[i - 1] = coord;
  }
  return true;
}