/**
 * @callback scopedElement
 * @param {HTMLElement}
 */
/**
 *
 * @param {string} tag
 * @param {scopedElement} scope
 * @returns {HTMLElement}
 */
export const n = (tag, scope) => {
    let newElement = document.createElement(tag);
    let sa = (k, v) => {
        newElement.setAttribute(k, v);
    };
    scope(newElement, sa);
    return newElement;
};
export const g = document.querySelector.bind(document);

export const toElement = str => {
    return new DOMParser().parseFromString(str, "text/xml").firstElementChild;
};
