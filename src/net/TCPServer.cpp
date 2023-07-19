#include "TCPServer.h"
#include "App.h"

#include "CLog.h"
EnHandleResult TCPServerListener::OnPrepareListen(ITcpServer* pSender,
                                                  SOCKET soListen) {
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPServerListener::OnAccept(ITcpServer* pSender, CONNID dwConnID,
                                           UINT_PTR soClient) {
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPServerListener::OnHandShake(ITcpServer* pSender,
                                              CONNID dwConnID) {
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPServerListener::OnReceive(ITcpServer* pSender,
                                            CONNID dwConnID, int iLength) {
  // Bytes buffer(iLength);
  // buffer.clear();
  // ITcpPullServer* pServer = ITcpPullServer::FromS(pSender);
  // EnFetchResult result = pServer->Fetch(dwConnID, buffer.ptr(), iLength);
  // std::cout << buffer.ptr() << std::endl;
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPServerListener::OnSend(ITcpServer* pSender, CONNID dwConnID,
                                         const BYTE* pData, int iLength) {
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPServerListener::OnClose(ITcpServer* pSender, CONNID dwConnID,
                                          EnSocketOperation enOperation,
                                          int iErrorCode) {
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPServerListener::OnShutdown(ITcpServer* pSender) {
  return HR_OK;
}