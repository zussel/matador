#include <matador/utils/identifier.hpp>

#include <matador/orm/persistence.hpp>
#include <matador/orm/session.hpp>

typedef unsigned long int  DWORD;

struct s_mediaserverconf
{
  matador::identifier<unsigned long> id;
  char DEV_NAME[255] = {};
  char strVhost[255] = {};
  char strApp[255] = {};
  char strId[255] = {};
  int wchannel = -1;
  DWORD dwStreamType = 0;

  double fDuration = 0.0;
  int bEanbleRtsp = 0;
  int bEanbleRtmp = 0;
  int bEanbleHls = 0;
  int bEnableMp4 = 0;
  int bUseAsynLogin = 0;
  int wPort = 0;
  char sDeviceAddress[255] = {};
  char sUserName[255] = {};
  char sPassword[255] = {};


  template<class SERIALIZER>
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("DEV_NAME", DEV_NAME, 255);
    serializer.serialize("strVhost", strVhost, 255);
    serializer.serialize("strApp", strApp, 255);
    serializer.serialize("strId", strId, 255);
    serializer.serialize("wchannel", wchannel);
    serializer.serialize("dwStreamType", dwStreamType);
    serializer.serialize("fDuration", fDuration);
    serializer.serialize("bEanbleRtsp", bEanbleRtsp);
    serializer.serialize("bEanbleRtmp", bEanbleRtmp);
    serializer.serialize("bEanbleHls", bEanbleHls);
    serializer.serialize("bEnableMp4", bEnableMp4);
    serializer.serialize("bUseAsynLogin", bUseAsynLogin);
    serializer.serialize("wPort", wPort);
    serializer.serialize("sDeviceAddress", sDeviceAddress, 255);
    serializer.serialize("sUserName", sUserName, 255);
    serializer.serialize("sPassword", sPassword, 255);
  }
};

using namespace matador;

int main() {
  persistence p("mysql://sascha:sascha@127.0.0.1/matador_sandbox");

  p.attach<s_mediaserverconf>("S_MEDIASERVERCONF");

  p.enable_log();
  p.create();

  session s(p);

  auto conf = s.insert(new s_mediaserverconf);
  conf.modify()->bEanbleHls = true;
  conf.modify()->bEnableMp4 = true;
  conf.modify()->bEanbleRtsp = false;
  conf.modify()->bEanbleRtmp = false;
  conf.modify()->wchannel = 3;
  conf.modify()->dwStreamType = 1234;
  conf.modify()->fDuration = 34.678;
  strcpy(conf.modify()->DEV_NAME, "light");
  strcpy(conf.modify()->strVhost, "192.168.11.23");
  conf.modify()->wPort = 8080;
  strcpy(conf.modify()->strApp, "LIGHT_SWITCH");
  strcpy(conf.modify()->strId, "LS1");
  strcpy(conf.modify()->sDeviceAddress, "1.2.3.4");
  strcpy(conf.modify()->sUserName, "admin");
  strcpy(conf.modify()->sPassword, "secret123");
  conf.modify()->bUseAsynLogin = true;

  s.flush();

  p.drop();
}
