/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014-2024                                                               *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#include <openspace/documentation/verifier.h>

#include <openspace/documentation/documentationengine.h>
#include <ghoul/misc/stringhelper.h>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace openspace::documentation {

// The explicit template instantiations for many of the commonly used template values
// This cuts down on the compilation time by only compiling these once
template class Vector2Verifier<int>;
template class Vector2Verifier<double>;
template class Vector3Verifier<int>;
template class Vector3Verifier<double>;
template class Vector4Verifier<int>;
template class Vector4Verifier<double>;

template class Vector2ListVerifier<int>;
template class Vector2ListVerifier<double>;
template class Vector3ListVerifier<int>;
template class Vector3ListVerifier<double>;
template class Vector4ListVerifier<int>;
template class Vector4ListVerifier<double>;

template class Matrix2x2Verifier<double>;
template class Matrix2x3Verifier<double>;
template class Matrix2x4Verifier<double>;
template class Matrix3x2Verifier<double>;
template class Matrix3x3Verifier<double>;
template class Matrix3x4Verifier<double>;
template class Matrix4x2Verifier<double>;
template class Matrix4x3Verifier<double>;
template class Matrix4x4Verifier<double>;

template class LessVerifier<IntVerifier>;
template class LessVerifier<DoubleVerifier>;
template class LessEqualVerifier<IntVerifier>;
template class LessEqualVerifier<DoubleVerifier>;
template class GreaterVerifier<IntVerifier>;
template class GreaterVerifier<DoubleVerifier>;
template class GreaterEqualVerifier<IntVerifier>;
template class GreaterEqualVerifier<DoubleVerifier>;
template class EqualVerifier<BoolVerifier>;
template class EqualVerifier<IntVerifier>;
template class EqualVerifier<DoubleVerifier>;
template class EqualVerifier<StringVerifier>;
template class UnequalVerifier<BoolVerifier>;
template class UnequalVerifier<IntVerifier>;
template class UnequalVerifier<DoubleVerifier>;
template class UnequalVerifier<StringVerifier>;

template class InListVerifier<BoolVerifier>;
template class InListVerifier<IntVerifier>;
template class InListVerifier<DoubleVerifier>;
template class InListVerifier<StringVerifier>;
template class NotInListVerifier<BoolVerifier>;
template class NotInListVerifier<IntVerifier>;
template class NotInListVerifier<DoubleVerifier>;
template class NotInListVerifier<StringVerifier>;

template class InRangeVerifier<IntVerifier>;
template class InRangeVerifier<DoubleVerifier>;
template class InRangeVerifier<DoubleVector2Verifier>;
template class InRangeVerifier<DoubleVector3Verifier>;
template class InRangeVerifier<DoubleVector4Verifier>;
template class InRangeVerifier<IntVector2Verifier>;
template class InRangeVerifier<IntVector3Verifier>;
template class InRangeVerifier<IntVector4Verifier>;
template class NotInRangeVerifier<IntVerifier>;
template class NotInRangeVerifier<DoubleVerifier>;
template class NotInRangeVerifier<DoubleVector2Verifier>;
template class NotInRangeVerifier<DoubleVector3Verifier>;
template class NotInRangeVerifier<DoubleVector4Verifier>;
template class NotInRangeVerifier<IntVector2Verifier>;
template class NotInRangeVerifier<IntVector3Verifier>;
template class NotInRangeVerifier<IntVector4Verifier>;

template class AnnotationVerifier<BoolVerifier>;
template class AnnotationVerifier<IntVerifier>;
template class AnnotationVerifier<DoubleVerifier>;
template class AnnotationVerifier<StringVerifier>;
template class AnnotationVerifier<TableVerifier>;
//template class AnnotationVerifier<BoolVector2Verifier>;
template class AnnotationVerifier<IntVector2Verifier>;
template class AnnotationVerifier<DoubleVector2Verifier>;
//template class AnnotationVerifier<BoolVector3Verifier>;
template class AnnotationVerifier<IntVector3Verifier>;
template class AnnotationVerifier<DoubleVector3Verifier>;
//template class AnnotationVerifier<BoolVector4Verifier>;
template class AnnotationVerifier<IntVector4Verifier>;
template class AnnotationVerifier<DoubleVector4Verifier>;

std::string BoolVerifier::type() const {
    return "Boolean";
}

std::string DoubleVerifier::type() const {
    return "Double";
}

TestResult IntVerifier::operator()(const ghoul::Dictionary& dict,
                                   const std::string& key) const
{
    if (dict.hasValue<int>(key)) {
        // We have a key and the value is int, we are done
        TestResult res;
        res.success = true;
        return res;
    }
    else {
        if (dict.hasKey(key)) {
            if (dict.hasValue<double>(key)) {
                // If we have a double value, we need to check if it is integer
                double value = dict.value<double>(key);
                double intPart;
                bool isInt = modf(value, &intPart) == 0.0;
                if (isInt) {
                    TestResult res;
                    res.success = true;
                    return res;
                }
                else {
                    TestResult res;
                    res.success = false;
                    TestResult::Offense o;
                    o.offender = key;
                    o.reason = TestResult::Offense::Reason::WrongType;
                    res.offenses.push_back(o);
                    return res;
                }
            }
            else {
                // If we don't have a double value, we cannot have an int value
                TestResult res;
                res.success = false;
                TestResult::Offense o;
                o.offender = key;
                o.reason = TestResult::Offense::Reason::WrongType;
                res.offenses.push_back(o);
                return res;
            }
        }
        else {
            TestResult res;
            res.success = false;
            TestResult::Offense o;
            o.offender = key;
            o.reason = TestResult::Offense::Reason::MissingKey;
            res.offenses.push_back(o);
            return res;
        }
    }
}

std::string IntVerifier::type() const {
    return "Integer";
}

StringVerifier::StringVerifier(bool mustBeNotEmpty)
    : _mustBeNotEmpty(mustBeNotEmpty)
{}

TestResult StringVerifier::operator()(const ghoul::Dictionary& dictionary,
                                      const std::string& key) const
{
    TestResult res = TemplateVerifier<std::string>::operator()(dictionary, key);
    if (!res.success) {
        return res;
    }

    std::string value = dictionary.value<std::string>(key);
    if (value.empty() && _mustBeNotEmpty) {
        res.success = false;
        res.offenses.push_back({
            key, TestResult::Offense::Reason::Verification, "value must not be empty"
        });
    }
    return res;
}

bool StringVerifier::mustBeNotEmpty() const {
    return _mustBeNotEmpty;
}

std::string StringVerifier::type() const {
    return "String";
}

IdentifierVerifier::IdentifierVerifier() : StringVerifier(true) {}

TestResult IdentifierVerifier::operator()(const ghoul::Dictionary& dict,
                                          const std::string& key) const
{
    TestResult res = StringVerifier::operator()(dict, key);
    if (!res.success) {
        return res;
    }

    std::string identifier = dict.value<std::string>(key);
    size_t pos = identifier.find_first_of(" \t\n\r.");
    if (pos != std::string::npos) {
        res.success = false;
        TestResult::Offense off;
        off.offender = key;
        off.reason = TestResult::Offense::Reason::Verification;
        off.explanation = "Identifier contained illegal character";
        res.offenses.push_back(off);
    }
    return res;
}

std::string IdentifierVerifier::documentation() const {
    return "An identifier string. May not contain '.', spaces, newlines, or tabs";
}

std::string IdentifierVerifier::type() const {
    return "Identifier";
}

FileVerifier::FileVerifier() : StringVerifier(true) {}

TestResult FileVerifier::operator()(const ghoul::Dictionary& dict,
                                    const std::string& key) const
{
    TestResult res = StringVerifier::operator()(dict, key);
    if (!res.success) {
        return res;
    }

    std::string file = dict.value<std::string>(key);
    if (!std::filesystem::exists(file) || !std::filesystem::is_regular_file(file)) {
        res.success = false;
        TestResult::Offense off;
        off.offender = key;
        off.reason = TestResult::Offense::Reason::Verification;
        off.explanation = "File did not exist";
        res.offenses.push_back(off);
    }
    return res;
}

std::string FileVerifier::type() const {
    return "File";
}

DirectoryVerifier::DirectoryVerifier() : StringVerifier(true) {}

TestResult DirectoryVerifier::operator()(const ghoul::Dictionary& dict,
                                         const std::string& key) const
{
    TestResult res = StringVerifier::operator()(dict, key);
    if (!res.success) {
        return res;
    }

    std::string dir = dict.value<std::string>(key);
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        res.success = false;
        TestResult::Offense off;
        off.offender = key;
        off.reason = TestResult::Offense::Reason::Verification;
        off.explanation = "Directory did not exist";
        res.offenses.push_back(off);
    }
    return res;
}

std::string DirectoryVerifier::type() const {
    return "Directory";
}

DateTimeVerifier::DateTimeVerifier() : StringVerifier(true) {}

TestResult DateTimeVerifier::operator()(const ghoul::Dictionary& dict,
                                        const std::string& key) const
{
    TestResult res = StringVerifier::operator()(dict, key);
    if (!res.success) {
        return res;
    }

    std::string dateTime = dict.value<std::string>(key);
    std::string format = "%Y %m %d %H:%M:%S"; // YYYY MM DD hh:mm:ss

    std::tm t = {};
    std::istringstream ss(dateTime);
    ss >> std::get_time(&t, format.c_str());

    // first check format (automatically checks if valid time)
    if (ss.fail()) {
        res.success = false;
        TestResult::Offense off;
        off.offender = key;
        off.reason = TestResult::Offense::Reason::Verification;
        off.explanation = "Not a valid format, should be: YYYY MM DD hh:mm:ss";
        res.offenses.push_back(off);
    }
    return res;
}

std::string DateTimeVerifier::type() const {
    return "Date and time";
}

TestResult Color3Verifier::operator()(const ghoul::Dictionary& dictionary,
                                      const std::string& key) const
{
    TestResult res = Vector3Verifier<double>::operator()(dictionary, key);
    if (!res.success) {
        return res;
    }

    glm::dvec3 values = dictionary.value<glm::dvec3>(key);
    if (values.x < 0.0 || values.x > 1.0) {
        res.success = false;
        TestResult::Offense o;
        o.offender = key + ".x";
        o.reason = TestResult::Offense::Reason::Verification;
        res.offenses.push_back(o);
    }

    if (values.y < 0.0 || values.y > 1.0) {
        res.success = false;
        TestResult::Offense o;
        o.offender = key + ".y";
        o.reason = TestResult::Offense::Reason::Verification;
        res.offenses.push_back(o);
    }

    if (values.z < 0.0 || values.z > 1.0) {
        res.success = false;
        TestResult::Offense o;
        o.offender = key + ".z";
        o.reason = TestResult::Offense::Reason::Verification;
        res.offenses.push_back(o);
    }

    return res;
}

std::string Color3Verifier::type() const {
    return std::string("Color3");
}

TestResult Color4Verifier::operator()(const ghoul::Dictionary& dictionary,
                                      const std::string& key) const
{
    TestResult res = Vector4Verifier<double>::operator()(dictionary, key);
    if (!res.success) {
        return res;
    }

    glm::dvec4 values = dictionary.value<glm::dvec4>(key);
    if (values.x < 0.0 || values.x > 1.0) {
        res.success = false;
        TestResult::Offense o;
        o.offender = key + ".x";
        o.reason = TestResult::Offense::Reason::Verification;
        res.offenses.push_back(o);
    }

    if (values.y < 0.0 || values.y > 1.0) {
        res.success = false;
        TestResult::Offense o;
        o.offender = key + ".y";
        o.reason = TestResult::Offense::Reason::Verification;
        res.offenses.push_back(o);
    }

    if (values.z < 0.0 || values.z > 1.0) {
        res.success = false;
        TestResult::Offense o;
        o.offender = key + ".z";
        o.reason = TestResult::Offense::Reason::Verification;
        res.offenses.push_back(o);
    }

    if (values.w < 0.0 || values.w > 1.0) {
        res.success = false;
        TestResult::Offense o;
        o.offender = key + ".a";
        o.reason = TestResult::Offense::Reason::Verification;
        res.offenses.push_back(o);
    }

    return res;
}

std::string Color4Verifier::type() const {
    return std::string("Color4");
}

template <>
TestResult TemplateVerifier<glm::ivec2>::operator()(const ghoul::Dictionary& dict,
                                                    const std::string& key) const
{
    if (dict.hasValue<glm::ivec2>(key)) {
        TestResult res;
        res.success = true;
        return res;
    }
    else {
        if (dict.hasKey(key)) {
            if (dict.hasValue<glm::dvec2>(key)) {
                glm::dvec2 value = dict.value<glm::dvec2>(key);
                glm::dvec2 intPart;
                glm::bvec2 isInt = glm::bvec2(
                    modf(value.x, &intPart.x) == 0.0,
                    modf(value.y, &intPart.y) == 0.0
                );
                if (isInt.x && isInt.y) {
                    TestResult res;
                    res.success = true;
                    return res;
                }
                else {
                    TestResult res;
                    res.success = false;
                    TestResult::Offense o;
                    o.offender = key;
                    o.reason = TestResult::Offense::Reason::WrongType;
                    res.offenses.push_back(o);
                    return res;
                }
            }
            else {
                TestResult res;
                res.success = false;
                TestResult::Offense o;
                o.offender = key;
                o.reason = TestResult::Offense::Reason::WrongType;
                res.offenses.push_back(o);
                return res;
            }
        }
        else {
            TestResult res;
            res.success = false;
            TestResult::Offense o;
            o.offender = key;
            o.reason = TestResult::Offense::Reason::MissingKey;
            res.offenses.push_back(o);
            return res;
        }
    }
}

template <>
TestResult TemplateVerifier<glm::ivec3>::operator()(const ghoul::Dictionary& dict,
                                                    const std::string& key) const
{
    if (dict.hasValue<glm::ivec3>(key)) {
        TestResult res;
        res.success = true;
        return res;
    }
    else {
        if (dict.hasKey(key)) {
            if (dict.hasValue<glm::dvec3>(key)) {
                glm::dvec3 value = dict.value<glm::dvec3>(key);
                glm::dvec3 intPart;
                glm::bvec3 isInt = glm::bvec3(
                    modf(value.x, &intPart.x) == 0.0,
                    modf(value.y, &intPart.y) == 0.0,
                    modf(value.z, &intPart.z) == 0.0
                );
                if (isInt.x && isInt.y && isInt.z) {
                    TestResult res;
                    res.success = true;
                    return res;
                }
                else {
                    TestResult res;
                    res.success = false;
                    TestResult::Offense o;
                    o.offender = key;
                    o.reason = TestResult::Offense::Reason::WrongType;
                    res.offenses.push_back(o);
                    return res;
                }
            }
            else {
                TestResult res;
                res.success = false;
                TestResult::Offense o;
                o.offender = key;
                o.reason = TestResult::Offense::Reason::WrongType;
                res.offenses.push_back(o);
                return res;
            }
        }
        else {
            TestResult res;
            res.success = false;
            TestResult::Offense o;
            o.offender = key;
            o.reason = TestResult::Offense::Reason::MissingKey;
            res.offenses.push_back(o);
            return res;
        }
    }
}

template <>
TestResult TemplateVerifier<glm::ivec4>::operator()(const ghoul::Dictionary& dict,
                                                    const std::string& key) const
{
    if (dict.hasValue<glm::ivec4>(key)) {
        TestResult res;
        res.success = true;
        return res;
    }
    else {
        if (dict.hasKey(key)) {
            if (dict.hasValue<glm::dvec4>(key)) {
                glm::dvec4 value = dict.value<glm::dvec4>(key);
                glm::dvec4 intPart;
                glm::bvec4 isInt = glm::bvec4(
                    modf(value.x, &intPart.x) == 0.0,
                    modf(value.y, &intPart.y) == 0.0,
                    modf(value.z, &intPart.z) == 0.0,
                    modf(value.w, &intPart.w) == 0.0
                );
                if (isInt.x && isInt.y && isInt.z && isInt.w) {
                    TestResult res;
                    res.success = true;
                    return res;
                }
                else {
                    TestResult res;
                    res.success = false;
                    TestResult::Offense o;
                    o.offender = key;
                    o.reason = TestResult::Offense::Reason::WrongType;
                    res.offenses.push_back(o);
                    return res;
                }
            }
            else {
                TestResult res;
                res.success = false;
                TestResult::Offense o;
                o.offender = key;
                o.reason = TestResult::Offense::Reason::WrongType;
                res.offenses.push_back(o);
                return res;
            }
        }
        else {
            TestResult res;
            res.success = false;
            TestResult::Offense o;
            o.offender = key;
            o.reason = TestResult::Offense::Reason::MissingKey;
            res.offenses.push_back(o);
            return res;
        }
    }
}

TableVerifier::TableVerifier(std::vector<DocumentationEntry> documentationEntries)
    : documentations(std::move(documentationEntries))
{}

TestResult TableVerifier::operator()(const ghoul::Dictionary& dictionary,
                                     const std::string& key) const
{
    if (dictionary.hasValue<Type>(key)) {
        ghoul::Dictionary d = dictionary.value<ghoul::Dictionary>(key);
        Documentation doc = { .entries = documentations };
        TestResult res = testSpecification(doc, d);

        // Add the 'key' as a prefix to make the new offender a fully qualified identifer
        for (TestResult::Offense& s : res.offenses) {
            s.offender = fmt::format("{}.{}", key, s.offender);
        }

        // Add the 'key' as a prefix to make the new warning a fully qualified identifer
        for (TestResult::Warning& w : res.warnings) {
            w.offender = fmt::format("{}.{}", key, w.offender);
        }

        return res;
    }
    else {
        if (dictionary.hasKey(key)) {
            TestResult res;
            res.success = false;
            TestResult::Offense o;
            o.offender = key;
            o.reason = TestResult::Offense::Reason::WrongType;
            res.offenses.push_back(o);
            return res;
        }
        else {
            TestResult res;
            res.success = false;
            TestResult::Offense o;
            o.offender = key;
            o.reason = TestResult::Offense::Reason::MissingKey;
            res.offenses.push_back(o);
            return res;
        }
    }
}

std::string TableVerifier::type() const {
    return "Table";
}

StringListVerifier::StringListVerifier(std::string elementDocumentation)
    : TableVerifier({
        { "*", new StringVerifier, Optional::No, std::move(elementDocumentation) }
    })
{}

std::string StringListVerifier::type() const {
    return "List of strings";
}

IntListVerifier::IntListVerifier(std::string elementDocumentation)
    : TableVerifier({
        { "*", new IntVerifier, Optional::No, std::move(elementDocumentation) }
    })
{}

std::string IntListVerifier::type() const {
    return "List of ints";
}

ReferencingVerifier::ReferencingVerifier(std::string id)
    : identifier(std::move(id))
{
    ghoul_assert(!identifier.empty(), "identifier must not be empty");
}

TestResult ReferencingVerifier::operator()(const ghoul::Dictionary& dictionary,
                                           const std::string& key) const
{
    TestResult res = TableVerifier::operator()(dictionary, key);
    if (res.success) {
        std::vector<Documentation> docs = DocEng.documentations();

        auto it = std::find_if(
            docs.begin(),
            docs.end(),
            [this](const Documentation& doc) { return doc.id == identifier; }
        );

        if (it == docs.end()) {
            res.success = false;
            TestResult::Offense o;
            o.offender = key;
            o.reason = TestResult::Offense::Reason::UnknownIdentifier;
            res.offenses.push_back(o);
            return res;
        }

        ghoul::Dictionary d = dictionary.value<ghoul::Dictionary>(key);
        TestResult r = testSpecification(*it, d);

        // Add the 'key' as a prefix to make the offender a fully qualified identifer
        for (TestResult::Offense& s : r.offenses) {
            s.offender = fmt::format("{}.{}", key, s.offender);
        }

        // Add the 'key' as a prefix to make the warning a fully qualified identifer
        for (TestResult::Warning& w : r.warnings) {
            w.offender = fmt::format("{}.{}", key, w.offender);
        }

        return r;
    }
    else {
        return res;
    }
}

std::string ReferencingVerifier::documentation() const {
    return "Referencing Documentation: '" + identifier + "'";
}

OrVerifier::OrVerifier(
            const std::vector<std::variant<Verifier*, std::shared_ptr<Verifier>>> values_)
{
    ghoul_assert(!values_.empty(), "values must not be empty");
    for (const std::variant<Verifier*, std::shared_ptr<Verifier>>& v : values_) {
        if (std::holds_alternative<Verifier*>(v)) {
            values.push_back(std::shared_ptr<Verifier>(std::get<Verifier*>(v)));
        }
        else {
            values.push_back(std::get<std::shared_ptr<Verifier>>(v));
        }
    }
}

TestResult OrVerifier::operator()(const ghoul::Dictionary& dictionary,
                                  const std::string& key) const
{
    std::vector<TestResult> res(values.size());
    std::transform(
        values.cbegin(),
        values.cend(),
        res.begin(),
        [dictionary, key](const std::shared_ptr<Verifier>& v) {
            return v->operator()(dictionary, key);
        }
    );

    const bool success = std::any_of(
        res.cbegin(),
        res.cend(),
        std::mem_fn(&TestResult::success)
    );

    if (success) {
        TestResult r;
        r.success = true;
        return r;
    }
    else {
        TestResult r;
        r.success = false;
        TestResult::Offense o;
        o.offender = key;
        o.reason = TestResult::Offense::Reason::Verification;
        r.offenses.push_back(o);
        return r;
    }
}

std::string OrVerifier::type() const {
    // Dirty hack to get an "or " inserted before the last element
    std::vector<std::string> types(values.size() - 1);
    std::transform(
        values.cbegin(),
        values.cend() - 1,
        types.begin(),
        std::mem_fn(&Verifier::type)
    );
    types.push_back(fmt::format("or {}", values.back()->type()));

    return ghoul::join(types, ", ");
}

std::string OrVerifier::documentation() const {
    // Dirty hack to get an "or " inserted before the last element
    std::vector<std::string> documentations(values.size() - 1);
    std::transform(
        values.cbegin(),
        values.cend() - 1,
        documentations.begin(),
        std::mem_fn(&Verifier::documentation)
    );
    documentations.push_back(fmt::format("or {}", values.back()->documentation()));

    return ghoul::join(documentations, ", ");
}

} // namespace openspace::documentation
