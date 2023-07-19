#include "TCPClient.h"

#include "App.h"
#include "CLog.h"

EnHandleResult TCPClientListener::OnPrepareConnect(ITcpClient* pSender,
                                                   CONNID dwConnID,
                                                   SOCKET socket) {
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPClientListener::OnConnect(ITcpClient* pSender,
                                            CONNID dwConnID) {
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPClientListener::OnHandShake(ITcpClient* pSender,
                                              CONNID dwConnID) {
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPClientListener::OnReceive(ITcpClient* pSender,
                                            CONNID dwConnID, int iLength) {
  // Bytes buffer(iLength);
  // buffer.clear();
  // ITcpPullClient* pClient = ITcpPullClient::FromS(pSender);
  // EnFetchResult result = pClient->Fetch(buffer.ptr(), iLength);
  // std::cout << buffer.ptr() << std::endl;
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPClientListener::OnSend(ITcpClient* pSender, CONNID dwConnID,
                                         const BYTE* pData, int iLength) {
  return HR_OK;
}
/***********************************************************************************************/
EnHandleResult TCPClientListener::OnClose(ITcpClient* pSender, CONNID dwConnID,
                                          EnSocketOperation enOperation,
                                          int iErrorCode) {
  return HR_OK;
}
/***********************************************************************************************/