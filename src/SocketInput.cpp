#include "SocketInput.h"

#include <I18n.h>
#include <thread>

#include "DasherInterfaceBase.h"
#include "DasherModel.h"

SocketInput::SocketInput(Dasher::CSettingsStore* pSettingsStore, Dasher::CDasherInterfaceBase* Controller, Dasher::CFrameRate* pFramerate): CScreenCoordInput(_("Socket Input")),
                                                                                                                                            CDefaultFilter(pSettingsStore, Controller, pFramerate, "Socket Mode")
{
    xLabel = m_pSettingsStore->GetStringParameter(Dasher::Parameter::SP_SOCKET_INPUT_X_LABEL);
    yLabel = m_pSettingsStore->GetStringParameter(Dasher::Parameter::SP_SOCKET_INPUT_Y_LABEL);
}

SocketInput::~SocketInput()
{
	stopListen();
}

bool SocketInput::GetScreenCoords(Dasher::screenint& iX, Dasher::screenint& iY, Dasher::CDasherView* pView)
{
	const Dasher::CDasherView::ScreenRegion screenRegion = pView->VisibleRegion();

	const double vectorLength = static_cast<double>(std::min(Dasher::CDasherModel::ORIGIN_Y - screenRegion.minY, screenRegion.maxY - Dasher::CDasherModel::ORIGIN_Y));
	const double normalization = sqrt(lastRelativeX * lastRelativeX + lastRelativeY * lastRelativeY);

	pView->Dasher2Screen(
		Dasher::CDasherModel::ORIGIN_X - static_cast<Dasher::myint>(lastRelativeX * (normalization > 1.0 ? vectorLength / normalization : vectorLength)),
		Dasher::CDasherModel::ORIGIN_Y - static_cast<Dasher::myint>(lastRelativeY * (normalization > 1.0 ? vectorLength / normalization : vectorLength)),
		iX, iY);

	return true;
}

void SocketInput::startListen()
{
	session_acceptor = std::make_unique<asio::ip::tcp::acceptor>(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), static_cast<short>(m_pSettingsStore->GetLongParameter(Dasher::LP_SOCKET_PORT))));

	receiveThread = std::thread([this](){

		asio::error_code ec;
		keepListenServerAlive = true;
		while(keepListenServerAlive)
		{
            open_socket = std::make_unique<asio::ip::tcp::socket>(session_acceptor->accept(ec));

			if(ec) continue; //error while accepting occured

		    keepReadThreadAlive = true;
			size_t bytes_read = 0;
		    while(keepReadThreadAlive)
		    {
			    if((bytes_read = read_until(*open_socket, stream_buffer, '\n', ec)))
			    {
                    std::string_view data{
                        static_cast<const char*>(stream_buffer.data().data()), 
                    stream_buffer.size()
                    };

					try	{
                        std::string_view cmd = data.substr(0, data.find(' '));
			            const double num = std::stod(data.data() + cmd.size()); //skip first two characters <cmd><whitespace>

						if(cmd == xLabel)
						{
						    lastRelativeX = num;
							relativeMode = true;
						}else if(cmd == yLabel)
						{
						    lastRelativeY = num;
							relativeMode = true;
						}else if(cmd == startStopLabel)
						{
							if(num >= 0.5)
							{
						       run(0);
							}else
							{
						       stop();
							}
						}
					} catch (std::exception&)
					{
                        std::cout << "Wrong format for socket input" << std::endl;
					}

					stream_buffer.consume(bytes_read);
			    } else if(ec) break;
		    }
			open_socket->close();
		}
	});
}

void SocketInput::stopListen()
{
	keepListenServerAlive = false;
	keepReadThreadAlive = false;
	if(session_acceptor) session_acceptor->close();
	if(open_socket) open_socket->close();
	if(receiveThread.joinable()) receiveThread.join();
}