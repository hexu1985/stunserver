/* include readline */
#include	"readline.h"

static bool compare_end(const std::string &str, const std::string &end_tag)
{
    return str.length() >= end_tag.length()
        && str.compare(str.length()-end_tag.length(), end_tag.length(), end_tag) == 0;
}

ssize_t readline(int sockfd, std::string &data, const std::string &end_tag)
{
    char buf[2048];
    int nread;

    data.clear();
    while (true) {
        if ((nread = recv(sockfd, buf, sizeof(buf), 0)) < 0) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        } else if (nread == 0)
            break;      // EOF

        data.append(buf, nread);
        if (compare_end(data, end_tag)) {
            data.erase(data.size() - end_tag.size(), end_tag.size());
            break;
        }
    }
    return data.length();
}

