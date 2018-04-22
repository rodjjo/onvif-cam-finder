#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <boost/uuid/sha1.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp>  // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>

#include "cam_finder/request_messages.h"


#define XML_DEVICES \
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" \
    "<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\"" \
    " xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\"" \
    " xmlns:tt=\"http://www.onvif.org/ver10/schema\">\n" \
    "    <s:Header>\n" \
    "        <Security s:mustUnderstand=\"1\" " \
    "xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-" \
    "wssecurity-secext-1.0.xsd\">\n" \
    "        <UsernameToken>\n" \
    "        <Username>%s</Username>\n" \
    "        <Password Type=\"http://docs.oasis-open.org/wss/2004/01/" \
    "oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">%s" \
    "</Password>\n" \
    "        <Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/" \
    "01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">%s" \
    "</Nonce>\n" \
    "        <Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/" \
    "oasis-200401-wss-wssecurity-utility-1.0.xsd\">%s</Created>\n" \
    "        </UsernameToken>\n" \
    "        </Security>\n" \
    "    </s:Header>\n" \
    "    <soap:Body>\n" \
    "        <trt:GetStreamUri>\n" \
    "            <trt:StreamSetup>\n" \
    "                <tt:Stream>%s</tt:Stream>\n" \
    "                <tt:Transport>\n" \
    "                    <tt:Protocol>%s</tt:Protocol>\n" \
    "                </tt:Transport>\n" \
    "            </trt:StreamSetup>\n" \
    "            <trt:ProfileToken>%s</trt:ProfileToken>\n" \
    "        </trt:GetStreamUri>\n" \
    "    </soap:Body>\n" \
    "</soap:Envelope>\n"

namespace camfinder {

std::string xml_encode(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for (size_t pos = 0; pos != data.size(); ++pos) {
        switch (data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}


std::string generate_xml(
    const std::string& user,
    const std::string& passwd_hash,
    const std::string& nonce,
    const std::string& created
) {
    const char *stream_type = "RTP-Unicast";
    const char *stream_protocol = "UDP";
    const char *stream_profile = "Profile1";

    char temp[20000];

    memset(temp, 0, sizeof(temp));

    snprintf(
        temp,
        sizeof(temp) - 1,
        XML_DEVICES,
        user.c_str(),
        passwd_hash.c_str(),
        nonce.c_str(),
        created.c_str(),
        stream_type,
        stream_protocol,
        stream_profile);
    return temp;
}

typedef struct {
    std::string username;
    std::string password;
    std::string nonce;
    std::string created;
} token_t;


std::string utc_date() {
    namespace bg = boost::gregorian;
    namespace pt = boost::posix_time;

    static char const* const date_format = "%Y-%m-%d";
    static char const* const time_format = "%H:%M:%S";

    auto today = bg::day_clock::universal_day();

    pt::ptime ptime(today, pt::second_clock::universal_time().time_of_day());

    std::ostringstream ss;
    ss.imbue(std::locale(std::cout.getloc(), new bg::date_facet(date_format)));
    ss << today << "T";
    ss.imbue(std::locale(std::cout.getloc(), new pt::time_facet(time_format)));
    ss << ptime << "Z";
    return ss.str();
}

std::string encode_base64(const char *data, unsigned int size) {
    namespace bai = boost::archive::iterators;

    typedef
        bai::base64_from_binary<
            bai::transform_width<const char *, 6, 8>
        >base64_text;

    std::ostringstream os;

    std::copy(
        base64_text(data),
        base64_text(data + size),
        bai::ostream_iterator<char>(os));

    std::string base64 = os.str();

    size_t remainder = base64.size() % 3;

    if (remainder != 0) {
        remainder = 3 - remainder;
        base64 += (remainder == 2 ? "==" : "=");
    }
    return base64;
}

typedef std::vector<char> byte_vector_t;
typedef char raw_hash_t[20];


void raw_sha1_of_vector(const byte_vector_t& data, raw_hash_t *result) {
    raw_hash_t & hash = *result;
    boost::uuids::detail::sha1 s;

    s.process_bytes(&data[0], data.size());
    unsigned int digest[5];
    s.get_digest(digest);

    for (int i = 0; i < 5; ++i) {
        const char* tmp = reinterpret_cast<char*>(digest);
        hash[i*4] = tmp[i*4+3];
        hash[i*4+1] = tmp[i*4+2];
        hash[i*4+2] = tmp[i*4+1];
        hash[i*4+3] = tmp[i*4];
    }
}

std::string sha1_base64(const byte_vector_t& data) {
    raw_hash_t hash;
    raw_sha1_of_vector(data, &hash);
    return encode_base64(hash, sizeof(hash));
}

const std::string sha1_of_vector(const byte_vector_t& data) {
    raw_hash_t hash;
    raw_sha1_of_vector(data, &hash);

    const char *hex_vals = "0123456789abcdef";

    char hex_hash[41];
    hex_hash[sizeof(hex_hash) - 1] = '\0';
    for (int i = 0; i < 20; ++i) {
        hex_hash[i * 2] = hex_vals[(hash[i] & 0x000000F0) >> 4];
        hex_hash[i * 2 + 1] = hex_vals[(hash[i] & 0x0000000F)];
    }

    return hex_hash;
}

std::string sha1_of_string(const std::string& data) {
    byte_vector_t vec(data.begin(), data.end());
    return sha1_of_vector(vec);
}

std::string create_nonce() {
    // auto uuid = boost::uuids::random_generator()();
    printf("base644 of admin = %s", encode_base64("admin", 5).c_str());
    return "noncecreated";
}


void generate_token(
    const std::string& username,
    const std::string& password,
    token_t * ptoken
) {
    token_t &token = *ptoken;
    token.username = xml_encode(username);
    token.created = utc_date();

    std::string nonce = create_nonce();

    token.nonce = encode_base64(&nonce[0], nonce.size());

    size_t necessary_size = nonce.size() +
        token.created.size() + password.size();

    byte_vector_t user_data;
    user_data.reserve(necessary_size);

    for (size_t i = 0; i < nonce.size(); ++i) {
        user_data.push_back(nonce[i]);
    }

    for (size_t i = 0; i < token.created.size(); ++i) {
        user_data.push_back(token.created[i]);
    }

    for (size_t i = 0; i < password.size(); ++i) {
        user_data.push_back(password[i]);
    }

    token.password = sha1_base64(user_data);
}

std::string query_stream_info_message(
    const std::string& host,
    const std::string& path,
    const std::string& username,
    const std::string& password
) {
    std::ostringstream data;

    data << "POST " << path << " HTTP/1.0\r\n";
    data << "Host: " << host << "\r\n";
    data << "Accept: */*\r\n";
    data << "Connection: close\r\n\r\n";

    token_t token;
    generate_token(username, password, &token);

    data << generate_xml(
        token.username, token.password, token.nonce, token.created);

    return data.str();
}

const char* get_discovery_message() {
    return "<?xml version=\"1.0\" ?>\n"  \
    "<s:Envelope xmlns:a=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"" \
        " xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\"" \
        " xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\n" \
    "        <s:Header>\n" \
    "            <a:Action>" \
                  "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe" \
                "</a:Action>\n" \
    "                <a:MessageID>" \
      "urn:uuid:2b0bf1e1-d725-49a9-834a-52656c4b5011</a:MessageID>\n" \
    "                <a:To>" \
      "urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>\n" \
    "        </s:Header>\n" \
    "        <s:Body>\n" \
    "                <d:Probe/>\n" \
    "        </s:Body>\n" \
    "</s:Envelope>\n";
}

}  // namespace camfinder
