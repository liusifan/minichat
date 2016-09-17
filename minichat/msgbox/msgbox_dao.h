#pragma once

#include "msgbox.pb.h"

namespace r3c {
  class CRedisClient;
};

class MsgBoxDAO {
 public:
  MsgBoxDAO( r3c::CRedisClient & client );
  ~MsgBoxDAO();

  int Add( const msgbox::MsgIndex & req,
          msgbox::AddMsgResp * resp );

  int GetBySeq( const msgbox::GetBySeqReq & req,
               msgbox::MsgIndexList * resp );

  int GetAll( const google::protobuf::StringValue & req,
             msgbox::MsgIndexList * resp );

 private:
  r3c::CRedisClient & client_;
};

