#include "CgiConnector.hpp"
//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------

// Need to delete[] return value.
const char**	CgiConnector::makeEnvp() const {
	const char**	envp = new char*[this->env_var.size() + 1];

	for (size_t i = 0; i < this->env_var.size(); ++i) {
		envp[i] = this->env_var[i].c_str();
	}
	envp[this->env_var.size()] = NULL;
	return envp;
}

//------------------------------------------------------------------------------
// Public Functions
//------------------------------------------------------------------------------

CgiConnector::CgiConnector(): env_var(0) {}

CgiConnector::CgiConnector(const CgiConnector& ref): env_var(ref.env_var) {}

CgiConnector::~CgiConnector() {}

CgiConnector&	CgiConnector::operator=(const CgiConnector& ref) {
	this->env_var = ref.env_var;
	return *this;
}
