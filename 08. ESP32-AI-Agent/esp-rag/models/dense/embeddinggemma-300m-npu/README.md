---
pipeline_tag: feature-extraction
tags:
- NPU
---
# EmbeddingGemma-300M (NPU)


## Model Description
**EmbeddingGemma** is a 300M-parameter open embedding model developed by **Google DeepMind**.  
It is built from **Gemma 3** (with T5Gemma initialization) and the same research and technology used in **Gemini models**.  

The model produces **vector representations of text**, making it well-suited for **search, retrieval, classification, clustering, and semantic similarity tasks**.  
It was trained on **100+ languages** with ~320B tokens, optimized for **on-device efficiency** (mobile, laptops, desktops).


## Features
- **Compact and efficient**: 300M parameters, optimized for on-device use.
- **Multilingual**: trained on 100+ spoken languages.
- **Flexible embeddings**: default dimension **768**, with support for **512, 256, 128** via Matryoshka Representation Learning (MRL).
- **Wide task coverage**: retrieval, QA, fact-checking, classification, clustering, similarity.
- **Commercial-friendly**: open weights available for research and production.


## Use Cases
- Semantic similarity and recommendation systems
- Document, code, and web search
- Clustering for organization, research, and anomaly detection
- Classification (e.g., sentiment, spam detection)
- Fact verification and QA embeddings
- Code retrieval for programming assistance


## Inputs and Outputs
**Input**:
- **Type**: Text string (e.g., query, prompt, document)  
- **Max Length**: 2048 tokens  

**Output**:
- **Type**: Embedding vector (default 768d)  
- **Options**: 512 / 256 / 128 dimensions via truncation & re-normalization (MRL)  


## Limitations & Responsible Use
This model has known limitations:  
- **Bias & coverage**: quality depends on training data diversity.  
- **Nuance & ambiguity**: may struggle with sarcasm, figurative language.  
- **Ethical concerns**: risk of bias perpetuation, privacy leakage, or malicious misuse.  

Mitigations:  
- CSAM and sensitive data filtering applied.  
- Users should adhere to **Gemma Responsible AI guidelines** and **Prohibited Use Policy**.  


## License  
This model is released under the **Creative Commons Attribution–NonCommercial 4.0 (CC BY-NC 4.0)** license.  
Non-commercial use, modification, and redistribution are permitted with attribution.  
For commercial licensing, please contact **dev@nexa.ai**.


## References
- [nexaSDK](https://sdk.nexa.ai)  


## Support
For SDK-related issues, visit [sdk.nexa.ai](https://sdk.nexa.ai).  
For model-specific questions, open an issue in this repository.