//
// Created by patrick on 27.12.21.
//

#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>

#include "convertedprogram.h"

namespace Graphics::OpenGL {

std::optional<GLint> ConvertedProgram::getAttributeLocation(const AttributeType &type) {
	const auto attributeIndexMapped = Program::getAttributeLocation(_attributeMappings[type]);
	if (attributeIndexMapped)
		return *attributeIndexMapped;

	return {};
}

std::optional<GLint> ConvertedProgram::getUniformLocation(const std::string &name) const {
	const auto symbolIter = _symbols.find(name);
	if (symbolIter != _symbols.end())
		return getUniformArraySymbolLocation(symbolIter->second);

	const auto samplerIter = _samplerMappings.find(name);
	if (samplerIter != _samplerMappings.end())
		return getUniformLocation(samplerIter->second);

	return Program::getUniformLocation(name);
}

void ConvertedProgram::addSamplerMappings(const std::map<std::string, std::string> &mappings) {
	_samplerMappings.insert(mappings.begin(), mappings.end());
}

void ConvertedProgram::setAttributeMappings(const std::map<AttributeType, std::string> &mappings) {
	_attributeMappings = mappings;
}

void ConvertedProgram::setSymbols(const std::vector<ShaderConverter::Symbol> &symbols) {
	for (const auto &symbol : symbols) {
		_symbols[symbol.name] = symbol;
	}
}

std::optional<GLint> ConvertedProgram::getUniformArraySymbolLocation(const ShaderConverter::Symbol &symbol, unsigned int offset) const {
	const std::string uniformArrayName =
			(symbol.shaderType == ShaderConverter::kVertex) ? "vs_uniforms_vec4" : "ps_uniforms_vec4";
	const std::string uniformArrayElementName = fmt::format("{}[{}]", uniformArrayName, symbol.index + offset);
	const auto uniformArrayLocation = getUniformLocation(uniformArrayElementName);
	if (!uniformArrayLocation)
		return {};

	return uniformArrayLocation;
}

void ConvertedProgram::setUniform1f(GLint id, const glm::vec1 &value) const {
	glm::vec4 alignedValue(value, 0.0f, 0.0f, 0.0f);
	Program::setUniform4f(id, alignedValue);
}

void ConvertedProgram::setUniform2f(GLint id, const glm::vec2 &value) const {
	glm::vec4 alignedValue(value, 0.0f, 0.0f);
	Program::setUniform4f(id, alignedValue);
}

void ConvertedProgram::setUniform3f(GLint id, const glm::vec3 &value) const {
	glm::vec4 alignedValue(value,  0.0f);
	Program::setUniform4f(id, alignedValue);
}

void ConvertedProgram::setUniformMatrix4f(GLint id, const glm::mat4 &value) const {
	glm::mat4 transposedValue = glm::transpose(value);
	glm::vec4 matRow1 = transposedValue[0];
	glm::vec4 matRow2 = transposedValue[1];
	glm::vec4 matRow3 = transposedValue[2];
	glm::vec4 matRow4 = transposedValue[3];
	Program::setUniform4f(id, matRow1);
	Program::setUniform4f(id + 1, matRow2);
	Program::setUniform4f(id + 2, matRow3);
	Program::setUniform4f(id + 3, matRow4);
}

}