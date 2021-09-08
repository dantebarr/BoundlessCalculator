#include <exception>
#include <string>

class DisabledFeatureException : public std::exception {
    public:
        DisabledFeatureException(std::string featureName);
        DisabledFeatureException(std::string featureName, std::string featureType);
	virtual const char* what() const throw();
        virtual ~DisabledFeatureException() throw () {}
    
    private:
	std::string message;
};
