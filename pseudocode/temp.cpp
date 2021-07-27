
Status		Client::readRequestMessage(std::vector<IoObject *>	&pool, int currentFd)
{
	char buf[IO_BUFFER_SIZE];
	ft_memset(buf, '\0', IO_BUFFER_SIZE + 1);
	int readLen = read(currentFd, buf, IO_BUFFER_SIZE);
	if (readLen <= 0)
		{
			if (readLen == 0)
			{
				return DISCONNECTED;
			}
			else
			{
				std::cerr << "Read failed " << this->fd << " client!!" << std::endl;
			}
		}
		buf[readLen] = '\0';

		// 1 이상이면 읽기에 성공한 것이니 읽어온 버퍼 저장
		this->readBuffer = this->readBuffer + buf;

		// header && body 파싱 가능한지 보고 파싱해주기
		if (this->status == REQUEST_RECEIVING_HEADER && this->headerParsable())
		{
			this->readBuffer = this->request.parseFirstLine(this->readBuffer);
			std::map<std::string, std::string> requestHeader = Request::parseHeader(this->readBuffer);
			this->request.setHeader(requestHeader);
			Location location = findLocation(*dynamic_cast<Server *>(pool[this->serverSocketFd]),
								this->request.getUri());
			this->request.setLocation(location);
			this->readBuffer = this->readBuffer.substr(this->readBuffer.find("\r\n\r\n") + 4);
			this->status = REQUEST_RECEIVING_BODY;
		}

		if (this->status == REQUEST_RECEIVING_BODY)
		{
			if (this->request.getHeader().count("Transfer-Encoding") == 1 &&
				this->request.getHeader()["Transfer-Encoding"] == "chunked")
			{
				size_t carrageIdx;
				carrageIdx = this->readBuffer.find("\r\n");
				while (true)
				{
					if (carrageIdx == std::string::npos)
						return REQUEST_RECEIVING_BODY;
					if (this->chunkedFlag == LEN)
					{
						int chunkedLen = ft_hex_atoi(this->readBuffer.substr(0, carrageIdx));
						this->bodyLen = chunkedLen;
						if (chunkedLen == 0)
						{
							if (this->readBuffer.length() >= carrageIdx + 4)
								this->readBuffer = this->readBuffer.substr(carrageIdx + 4);
							else
								this->readBuffer.clear();
							return RESPONSE_READY;
						}
						if (this->readBuffer.length() >= carrageIdx + 2)
						{
							this->readBuffer = this->readBuffer.substr(carrageIdx + 2);
							this->chunkedFlag = BODY;
						}
					}
					else if (this->chunkedFlag == BODY)
					{
						if (this->readBuffer.length() >= this->bodyLen + 2)
						{
							this->writeBuffer += this->readBuffer.substr(0, this->bodyLen);
							this->chunkedFlag = LEN;
							this->readBuffer = this->readBuffer.substr(this->bodyLen + 2);
						}
						else
							return REQUEST_RECEIVING_BODY;
					}
					carrageIdx = this->readBuffer.find("\r\n");
				}
			}
			else if (this->request.getHeader().count("Content-Length") == 1)
			{
				int contentLength = ft_atoi(this->request.getHeader()["Content-Length"].c_str());
				if (this->readBuffer.length() >= (size_t)contentLength)
				{
					this->writeBuffer = this->readBuffer.substr(0, contentLength);
					if (this->readBuffer.length() >= (size_t)contentLength + 4)
						this->readBuffer = this->readBuffer.substr(contentLength + 4);
					else
						this->readBuffer.clear();
					return RESPONSE_READY;
				}
				else
					return REQUEST_RECEIVING_BODY;
			}
			else if (this->request.getHeader().count("Content-Length") == 0)
			{
				this->request.getHeader()["Content-Length"] = "0";
				this->readBuffer.clear();
				return RESPONSE_READY;
			}
		}

	return this->status;

}
